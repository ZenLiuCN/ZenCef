package ws

import (
	"context"
	"encoding/hex"
	"errors"
	"gabs"
	ws "github.com/gorilla/websocket"
	"io"
	"logger"
	"net"
	"net/http"
	"strconv"
	"time"
)

var (
	log *logger.Logger
)

func SetLogger(logger *logger.Logger) {
	log = logger
}

var (
	ErrRunning      = errors.New("already running")
	ErrNoConn       = errors.New("no such connection ")
	ErrCodecConfilt = errors.New("codec must the same as server or client")
	ErrNoDialed     = errors.New("client not dialed")
)

/*type Pool int

func (this *Pool) Get() (r interface{}) {
	r, _ = pool.DefaultPool.Get(1)
	return
}
func (this *Pool) Put(r interface{}) {
	pool.DefaultPool.Put(r, 1)
}*/
type WsCloseHandler func(addr net.Addr)
type WsHandler func(msg *gabs.Container) (*gabs.Container, error)
type WsDefaultHandler func(id string, msg *gabs.Container) (*gabs.Container, error)

type Channel struct {
	conn         *ws.Conn
	ctx          context.Context
	cancel       context.CancelFunc
	processor    WsDefaultHandler
	callback     map[string]WsHandler
	closeHandler WsCloseHandler
}

func (this *Channel) Destroy() {
	this.SendClose(nil)
	this.cancel()
	//this.conn = nil
}
func (this *Channel) Listen(processor WsDefaultHandler) {
	this.callback = make(map[string]WsHandler)
	this.processor = processor
	go func() {
		defer func() {
			log.Tracef("handler is %+v", this.closeHandler)
			if this.closeHandler != nil {
				if this.conn != nil {
					this.closeHandler(this.conn.RemoteAddr())
				} else {
					this.closeHandler(nil)
				}
			}
			if this.conn != nil {
				this.conn.Close()
				this.conn = nil
			}
		}()
		for {
			select {
			case <-this.ctx.Done():
				return
			default:
				mt, m, e := this.conn.ReadMessage()
				if e != nil {
					if e == io.EOF {
						continue
					} else if _, ok := e.(*ws.CloseError); ok {
						return
					} else {
						log.Errorf("error read ws message\n %v", e)
						return
					}
				}
				switch mt {
				case ws.BinaryMessage:
					if this.handler(m) != nil {
						return
					}
				case ws.CloseMessage:
					return
				default:
					log.Infof("unsupported message type %d from %+v \n %+v", mt, this.conn.RemoteAddr(), hex.Dump(m))
					continue
				}
			}

		}
	}()
}
func (this *Channel) WriteRaw(data []byte, mtype int) error {
	return this.conn.WriteMessage(mtype, data)
}
func (this *Channel) WriteError(data error) error {
	return this.WriteErrorFrame(GetMessageId(), data)
}
func (this *Channel) WriteErrorFrame(id string, data error) error {
	c := gabs.New()
	c.Set(false, `status`)
	c.Set(data.Error(), `message`)
	return this.WriteFrame(id, c)
}
func (this *Channel) WriteFrame(id string, msg *gabs.Container) error {
	c := gabs.New()
	c.Set(id, `id`)
	c.Set(msg.Data(), `data`)
	return this.WriteRaw(c.Bytes(), ws.BinaryMessage)
}
func (this *Channel) WriteFrameCallback(id string, msg *gabs.Container, callback WsHandler) error {
	if er := this.WriteFrame(id, msg); er != nil {
		return er
	}
	this.callback[id] = callback
	return nil
}
func (this *Channel) SendCallback(msg *gabs.Container, callback WsHandler) error {
	return this.WriteFrameCallback(GetMessageId(), msg, callback)
}
func (this *Channel) Send(msg *gabs.Container) error {
	return this.WriteFrame(GetMessageId(), msg)
}
func (this *Channel) SendClose(msg *[]byte) error {
	if msg != nil {
		return this.WriteRaw(*msg, ws.CloseMessage)
	} else {
		return this.WriteRaw([]byte{}, ws.CloseMessage)
	}

}
func (this *Channel) SetOnClose(handler WsCloseHandler) {
	this.closeHandler = handler
}
func (this *Channel) RemoveOnClose() {
	this.closeHandler = nil
}
func (this *Channel) handler(bits []byte) (e error) {
	if len(bits) == 0 {
		return nil
	}
	var data *gabs.Container
	if data, e = gabs.ParseJSON(bits); e != nil {
		log.Errorf(`error decode message of %X error %v`, bits, e)
		this.WriteError(errors.New(`protocal error`))
		return nil
	}
	log.Tracef("received data %+v", data)
	id := ""
	if id = data.GetString(`id`); id == "" {
		log.Errorf("message  find no id in %+v", data)
		this.WriteError(errors.New(`protocal error`))
		return nil
	}

	if !data.Exists(`data`) {
		log.Errorf("message find no data in %+v", data)
		this.WriteError(errors.New(`protocal error`))
		return nil
	}

	if f, ok := this.callback[id]; ok {
		defer delete(this.callback, id)
		msg, err := f(data.Path(`data`))
		if msg != nil {
			return this.WriteFrame(id, msg)
		} else if err != nil {
			return this.WriteErrorFrame(id, err)
		}
		return nil
	}
	msg, err := this.processor(id, data.Path(`data`))
	if msg != nil {
		return this.WriteFrame(id, msg)
	} else if err != nil {
		return this.WriteErrorFrame(id, err)
	}
	return nil
}
func NewChannel(conn *ws.Conn, handler WsDefaultHandler, ctx context.Context, cancel context.CancelFunc) *Channel {
	ch := &Channel{conn: conn, ctx: ctx, cancel: cancel}
	ch.Listen(handler)
	return ch
}

type WsSrv struct {
	server             *http.Server
	ctx                context.Context
	upgrader           *ws.Upgrader
	cancel             context.CancelFunc
	channel            map[net.Addr]*Channel
	handler            WsDefaultHandler
	client             *Channel
	clientCloseHandler WsCloseHandler
}

func (this *WsSrv) Dial(uri string, handler WsDefaultHandler) error {
	if this.client != nil {
		this.client.Destroy()
	}
	ws.DefaultDialer.HandshakeTimeout = 1500 * time.Millisecond
	ws.DefaultDialer.WriteBufferSize = 4096
	ws.DefaultDialer.ReadBufferSize = 4096
	conn, _, er := ws.DefaultDialer.Dial(uri, nil)
	if er != nil {
		log.Errorf("fail to dial to server %v", er)
		return er
	}
	ctx, cancel := context.WithCancel(context.Background())
	this.client = NewChannel(conn, handler, ctx, cancel)
	return nil
}
func (this *WsSrv) WriteRaw(data []byte, mtype int) error {
	if this.client == nil {
		return ErrNoDialed
	}
	return this.client.WriteRaw(data, mtype)
}
func (this *WsSrv) WriteFrame(id string, msg *gabs.Container) error {
	if this.client == nil {
		return ErrNoDialed
	}
	return this.client.WriteFrame(id, msg)
}
func (this *WsSrv) WriteFrameCallback(id string, msg *gabs.Container, callback WsHandler) error {
	if this.client == nil {
		return ErrNoDialed
	}
	return this.client.WriteFrameCallback(id, msg, callback)
}
func (this *WsSrv) Send(msg *gabs.Container) error {
	return this.client.Send(msg)
}
func (this *WsSrv) SendCallback(msg *gabs.Container, callback WsHandler) error {
	return this.client.SendCallback(msg, callback)
}
func (this *WsSrv) SendClose(msg *[]byte) error {
	if msg != nil {
		return this.WriteRaw(*msg, ws.CloseMessage)
	} else {
		return this.WriteRaw([]byte{}, ws.CloseMessage)
	}

}
func (this *WsSrv) SetOnClientClose(handler WsCloseHandler) {
	this.client.closeHandler = handler
}
func (this *WsSrv) DestroyClient() {
	if this.client != nil {
		this.client.Destroy()
	}
}
func (this WsSrv) HasClient() bool {
	return this.client != nil && this.client.conn != nil
}

/**
uri :port
*/
func (this *WsSrv) Serve(addr, path string, handler WsDefaultHandler) error {
	this.channel = make(map[net.Addr]*Channel)
	this.ctx, this.cancel = context.WithCancel(context.Background())
	this.handler = handler
	this.upgrader = &ws.Upgrader{
		HandshakeTimeout: 2 * time.Second,
		ReadBufferSize:   4096,
		WriteBufferSize:  4096,
	}
	this.server = &http.Server{
		ReadHeaderTimeout: time.Millisecond,
		WriteTimeout:      2 * time.Second,
	}
	http.HandleFunc(path, func(w http.ResponseWriter, r *http.Request) {
		c, er := this.upgrader.Upgrade(w, r, nil)
		if er != nil {
			log.Errorf("error upgrade ws %v", er)
		}
		this.newChannel(c)
		return
	})
	if addr != "" {
		this.server.Addr = addr
		go func() {
			er := this.server.ListenAndServe()
			log.Errorf("service start error %v", er)
		}()

	}
	return nil
}

func (this *WsSrv) SrvWriteRaw(addr *net.Addr, data []byte, mtype int) error {
	if addr == nil {
		for _, v := range this.channel {
			e := v.WriteRaw(data, mtype)
			if e != nil {
				return e
			}
		}
	} else {
		if v, ok := this.channel[*addr]; ok {
			return v.WriteRaw(data, mtype)
		} else {
			return ErrNoConn
		}
	}
	return nil
}
func (this *WsSrv) SrvWriteFrame(addr *net.Addr, id string, msg *gabs.Container) error {
	if addr == nil {
		for _, v := range this.channel {
			e := v.WriteFrame(id, msg)
			if e != nil {
				return e
			}
		}
	} else {
		if v, ok := this.channel[*addr]; ok {
			return v.WriteFrame(id, msg)
		} else {
			return ErrNoConn
		}
	}
	return nil
}
func (this *WsSrv) SrvWriteFrameCallback(addr *net.Addr, id string, msg *gabs.Container, callback WsHandler) error {
	if addr == nil {
		for _, v := range this.channel {
			e := v.WriteFrameCallback(id, msg, callback)
			if e != nil {
				return e
			}
		}
	} else {
		if v, ok := this.channel[*addr]; ok {
			return v.WriteFrameCallback(id, msg, callback)
		} else {
			return ErrNoConn
		}
	}

	return nil
}
func (this *WsSrv) SrvSend(addr *net.Addr, msg *gabs.Container) error {
	return this.SrvWriteFrame(addr, GetMessageId(), msg)
}
func (this *WsSrv) SrvSendCallback(addr *net.Addr, msg *gabs.Container, callback WsHandler) error {
	return this.SrvWriteFrameCallback(addr, GetMessageId(), msg, callback)
}
func (this *WsSrv) SetOnClientDisconnect(handler WsCloseHandler) {
	this.clientCloseHandler = handler
}
func (this *WsSrv) DestroyServer() {
	if this.server != nil {
		for _, value := range this.channel {
			value.Destroy()
		}
		this.cancel()
		this.server.Close()
		this.server = nil
	}
}
func (this WsSrv) HasServer() bool {
	return this.server != nil
}
func (this *WsSrv) Destroy() {
	this.DestroyClient()
	this.DestroyServer()
}
func (this *WsSrv) newChannel(conn *ws.Conn) {
	ctx, cancel := context.WithCancel(this.ctx)
	this.channel[conn.RemoteAddr()] = NewChannel(conn, this.handler, ctx, cancel)
	if this.clientCloseHandler != nil {
		this.channel[conn.RemoteAddr()].closeHandler = this.clientCloseHandler
	}
}
func GetMessageId() string {
	return strconv.FormatInt(time.Now().UnixNano(), 10)
}
