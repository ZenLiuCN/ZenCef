package service

import (
	"context"
	mux2 "github.com/gorilla/mux"
	"github.com/gorilla/websocket"
	"net/http"
	"time"
)

var (
	srv    = make(map[string]Service)
	mux    *mux2.Router
	Server *http.Server
)

func init() {
	mux = mux2.NewRouter()
}

type Service interface {
	Name() string
	ServeHTTP(http.ResponseWriter, *http.Request)
	Disable()
	Enable()
	Status() bool
	Register(*mux2.Router)
}

func RegisterService(service Service) {
	service.Register(mux)
	srv[service.Name()] = service
}
func DisableService(name string) {
	if s, ok := srv[name]; ok {
		s.Disable()
	}
}
func EnableService(name string) {
	if s, ok := srv[name]; ok {
		s.Enable()
	}
}
func ServiceNames() (keys []string) {
	keys = make([]string, 0, len(srv))
	for k := range srv {
		keys = append(keys, k)
	}
	return
}
func ServiceStatus() (ser map[string]bool) {
	ser = make(map[string]bool)
	for key, value := range srv {
		ser[key] = value.Status()
	}
	return
}

type ServiceFunc struct {
	name    string
	path    string
	enable  bool
	Handler http.HandlerFunc
}

func (s ServiceFunc) Name() string {
	return s.name
}
func (s ServiceFunc) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	if !s.enable {
		w.WriteHeader(404)
		return
	}
	s.Handler(w, r)
}
func (s ServiceFunc) Disable() {
	s.enable = false
}
func (s ServiceFunc) Enable() {
	s.enable = true
}
func (s ServiceFunc) Status() bool {
	return s.enable
}
func (s ServiceFunc) Register(r *mux2.Router) {
	r.Path(s.path).Handler(&s)
}
func NewService(name string, path string, handlerFunc http.HandlerFunc) *ServiceFunc {
	s := new(ServiceFunc)
	s.name = name
	s.path = path
	s.enable = true
	s.Handler = handlerFunc
	RegisterService(s)
	return s
}
func NewServiceHandler(name string, path string, handlerFunc http.Handler) {
	mux.Path(path).Handler(handlerFunc)
}
func NewResourceService(path string, handlerFunc http.Handler) {
	mux.PathPrefix(path).Handler(handlerFunc)
}
func NewWebsocketService(name string, path string, handler WsHandlerFunction, readBufferSize, writeBufferSize int) *ServiceFunc {
	if readBufferSize == 0 {
		readBufferSize = 1024
	}
	if writeBufferSize == 0 {
		writeBufferSize = 1024
	}
	s := new(ServiceFunc)
	s.name = name
	s.path = path
	s.enable = true
	s.Handler = func(w http.ResponseWriter, r *http.Request) {
		log.Infof("Get Ws Connection ")
		var upgrader = websocket.Upgrader{
			ReadBufferSize:  readBufferSize,
			WriteBufferSize: writeBufferSize,
			CheckOrigin:     func(r *http.Request) bool { return true },
		}
		conn, err := upgrader.Upgrade(w, r, nil)
		if err != nil {
			handler(nil, err, -1, nil)
			return
		}
		c := NewWsConnJson(conn)
		handler(c, nil, 0, nil)
		for {
			messageType, r, err := conn.NextReader()
			if err != nil {
				if e := handler(c, err, messageType, r); e != nil {
					return
				} else {
					continue
				}
			}
			if e := handler(c, err, messageType, r); e != nil {
				return
			} else {
				continue
			}
		}
	}
	RegisterService(s)
	return s
}
func NewMiddleWare(fun mux2.MiddlewareFunc) {
	mux.Use(fun)
}
func ListenAndServe(addr string) {
	mux.Use(loggerMiddleWare)
	Server = &http.Server{
		Addr:         addr,
		Handler:      mux,
		WriteTimeout: time.Second * 15,
		ReadTimeout:  time.Second * 15,
		IdleTimeout:  time.Second * 60,
	}
	go func() {
		if er := Server.ListenAndServe(); er != nil {
			panic(er)
			Server = nil
		}
	}()
}
func Shutdown() error {
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*15)
	defer cancel()
	e := Server.Shutdown(ctx)
	Server = nil
	return e
}
func IsRunning() bool {
	if Server == nil {
		return false
	}
	return true
}
func loggerMiddleWare(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		// Do stuff here
		//log.Tracef("incoming request %+v from %+v \n %+v",r.RequestURI,r.RemoteAddr,r.Header)
		log.Infof("incoming request %+v from %+v ", r.RequestURI, r.RemoteAddr)
		// Call the next handler, which can be another middleware in the chain, or the final handler.
		next.ServeHTTP(w, r)
	})
}
func HasService(name string) bool {
	for k := range srv {
		if k == name {
			return true
		}
	}
	return false
}
func HasEnableService(name string) bool {
	for k, v := range srv {
		if k == name && v.Status() {
			return true
		}
	}
	return false
}
