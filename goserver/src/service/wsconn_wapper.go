package service

import (
	"encoding/hex"
	"errors"
	"gabs"
	"github.com/gorilla/websocket"
	"github.com/ugorji/go/codec"
	"runtime/debug"
	"strconv"
	"time"
)

type CODEC int

const (
	NULL CODEC = iota
	JSON
	MSGPK
	BINC
	CBOR
)

func (e CODEC) String() string {
	switch e {
	case JSON:
		return "JSON "
	case MSGPK:
		return "MSGPK"
	case BINC:
		return "BINC"
	case CBOR:
		return "CBOR"
	default:
		return "NULL"
	}
}
func (e CODEC) Handler() codec.Handle {
	switch e {
	case JSON:
		return new(codec.JsonHandle)
	case MSGPK:
		return new(codec.MsgpackHandle)
	case BINC:
		return new(codec.BincHandle)
	case CBOR:
		return new(codec.CborHandle)
	default:
		return nil
	}
}

type WsCallback func(conn *WsConnJson, msg *gabs.Container) error
type WsConnCodec struct {
	*websocket.Conn
	code     CODEC
	decoder  *codec.Decoder
	encoder  *codec.Encoder
	callback map[string]WsCallback
}

func (this WsConnCodec) WriteRaw(data []byte, mtype int) error {
	return this.WriteMessage(mtype, data)
}
func (this WsConnCodec) WriteError(data error) error {
	return this.WriteErrorFrame(GetMessageId(), data)
}
func (this WsConnCodec) WriteErrorFrame(id string, data error) error {
	g := gabs.New()
	g.Set(false, `status`)
	g.Set(data.Error(), `message`)
	return this.WriteFrame(id, g)
}
func (this WsConnCodec) WriteFrame(id string, msg *gabs.Container) error {
	var data []byte
	this.encoder.ResetBytes(&data)
	if e := this.encoder.Encode(map[string]interface{}{
		`id`:   id,
		`data`: msg,
	}); e != nil {
		return e
	}
	return this.WriteRaw(data, websocket.BinaryMessage)
}
func (this WsConnCodec) WriteFrameCallback(id string, msg *gabs.Container, callback WsCallback) error {
	if er := this.WriteFrame(id, msg); er != nil {
		return er
	}
	this.callback[id] = callback
	return nil
}

func GetMessageId() string {
	return strconv.FormatInt(time.Now().UnixNano(), 10)
}

var (
	ErrNoCallback = errors.New("No Callback ")
)

type WsConnJson struct {
	*websocket.Conn
	callback map[string]WsCallback
}

func (this WsConnJson) GetConn() *websocket.Conn {
	return this.Conn
}
func (this WsConnJson) WriteRaw(data []byte, mtype int) error {
	log.Tracef("ws send raw type %d \n%+v", mtype, hex.Dump(data))
	return this.WriteMessage(mtype, data)
}
func (this WsConnJson) WriteError(data error) error {
	return this.WriteErrorFrame(GetMessageId(), data)
}
func (this WsConnJson) WriteErrorFrame(id string, data error) error {
	g := gabs.New()
	g.Set(false, `status`)
	g.Set(data.Error(), `message`)
	return this.WriteFrame(id, g)
}
func (this WsConnJson) WriteFrame(id string, msg *gabs.Container) (er error) {
	if msg.IsArray() {
		g := gabs.New()
		g.Set(msg, `data`)
		g.Set(id, `id`)
		return this.WriteRaw(g.Bytes(), websocket.TextMessage)
	}
	//log.Tracef("ws send frame %s %v", id, msg)
	if _, e := msg.GetString(`id`); e != nil {
		_, er = msg.Set(id, `id`)
		if er != nil {
			log.Errorf("send frame error %+v \n%s", er, string(debug.Stack()))
		}
	}
	log.Tracef("ws send frame %s %v", id, msg)
	return this.WriteRaw(msg.Bytes(), websocket.TextMessage)
}
func (this WsConnJson) WriteFrameCallback(id string, msg *gabs.Container, callback WsCallback) error {
	if er := this.WriteFrame(id, msg); er != nil {
		return er
	}
	this.callback[id] = callback
	return nil
}

func (this WsConnJson) Send(msg *gabs.Container) error {
	return this.WriteFrame(GetMessageId(), msg)
}
func (this WsConnJson) SendRaw(msg *gabs.Container) error {
	return this.WriteRaw(msg.Bytes(), websocket.TextMessage)
}
func (this WsConnJson) SendResponse(id string, msg *gabs.Container) error {
	return this.WriteFrame(id, msg)
}
func (this WsConnJson) SendResponseDone(id string, msg *gabs.Container) error {
	msg=warpToMap(msg)
	msg.Set(true,`status`)
	return this.WriteFrame(id, msg)
}
func (this WsConnJson) SendResponseSuccess(id string) error {
	g := gabs.New()
	g.Set(true, `status`)
	return this.WriteFrame(id, g)
}
func (this WsConnJson) SendError(err error) error {
	return this.WriteErrorFrame(GetMessageId(), err)
}
func (this WsConnJson) SendErrorResponse(id string, err error) error {
	return this.WriteErrorFrame(id, err)
}
func (this WsConnJson) SendCallback(msg *gabs.Container, callback WsCallback) error {
	return this.WriteFrameCallback(GetMessageId(), msg, callback)
}
func (this WsConnJson) HandleCallback(msg *gabs.Container) error {
	id, _ := msg.GetString(`id`)
	if id == "" {
		return ErrNoCallback
	}
	if f, ok := this.callback[id]; !ok {
		return ErrNoCallback
	} else {
		return f(&this, msg)
	}
}
func NewWsConnJson(ws *websocket.Conn) *WsConnJson {
	return &WsConnJson{Conn: ws}
}
func warpToMap(g *gabs.Container)*gabs.Container{
	if g.IsArray(){
		g1:=gabs.New()
		g1.Set(g.Data(),`data`)
		return g1
	}
	return g
}