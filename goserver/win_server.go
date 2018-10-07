package main

/*
#include <windows.h>
*/
import "C"
import (
	"bytes"
	"github.com/gorilla/websocket"
	. "github.com/lxn/win"
	"net/http"
	"os"
	"strconv"
	"strings"
	"unsafe"
)

var (
	apiSrv        *http.Server
	service       = make(map[string]bool)
	serverRunning = false
)

//export goStopServer
func goStopServer() {
	if debug != 0 {
		if debug != 0 {
			logger.Println("called goStopServer")
		}
	}
	apiSrv.Close()
}

//export goGetExtJson
func goGetExtJson() *C.char {
	if debug != 0 {
		logger.Println("called goGetExtJson")
	}
	return C.CString(`var WinObj;WinObj||(WinObj={});(function(){WinObj.wss="ws://127.0.0.1:65530/win";WinObj.close=function(){WinObj.WinWS.send("win:close")};WinObj.full=function(){WinObj.WinWS.send("win:full")};WinObj.topMost=function(){WinObj.WinWS.send("win:topMost")};WinObj.max=function(){WinObj.WinWS.send("win:max")};WinObj.min=function(){WinObj.WinWS.send("win:min")};WinObj.restore=function(){WinObj.WinWS.send("win:restore")};WinObj.drag=function(a){a?WinObj.WinWS.send("win:drag:start"):WinObj.WinWS.send("win:drag:stop")};WinObj.onMouseMove=function(a){eve=window.event||ev;WinObj.WinWS.send("win:drag:move|"+eve.offsetX+"|"+eve.offsetY)}})();`)
}

//export goIsServerStarted
func goIsServerStarted() int {
	if serverRunning {
		return 1
	} else {
		return 0
	}
}

//export goUseHttpServer
func goUseHttpServer(root *C.char) int {
	dir := C.GoString(root)
	if debug != 0 {
		logger.Printf("will enable http service of %s ", dir)
	}
	if i, e := os.Stat(dir); e != nil {
		if debug != 0 {
			logger.Printf("check dir error: %s ", e)
		}
		return 0
	} else if !i.IsDir() {
		if debug != 0 {
			logger.Printf("check dir error: not dir of %s ", i)
		}
		return -1
	}
	if debug != 0 {
		logger.Printf("enable http service of %s ", dir)
	}
	http.Handle("/", http.FileServer(http.Dir(dir)))
	service["dir"] = true
	return 1
}

//export goRunSchemeCommand
func goRunSchemeCommand(url *C.char) {
	uri := C.GoString(url)
	act := strings.Split(strings.Replace(strings.ToLower(uri), "window://", "", -1), "/")
	if debug != 0 {
		logger.Printf("scheme command  %s \n", act)
	}
	if len(act) == 0 {
		logger.Printf("scheme command of  %s  is empty\n", uri)
		return
	}
	if win == HWND(0) {
		logger.Printf("scheme command %s not set window handler of %p \n", uri, unsafe.Pointer(win))
		return
	}
	switch act[0] {
	case "close":
		setClose()
	case "full":
		setFullWindow()
	case "max":
		setMaximize()
	case "min":
		setMinimize()
	case "restore":
		setRestore()
	case "topmost":
		setTopMost()
	case "nonetop":
		setNoneTop()
	case "drag":
		setBeginDrag()
	case "drop":
		setEndDrag()
	case "move":
		if len(act) == 3 {
			X, e1 := strconv.Atoi(act[1])
			Y, e2 := strconv.Atoi(act[2])
			if e1 != nil || e2 != nil {
				return
			}
			doMoveWindow(X, Y)
		}
	case "thin":
		setFrameThin()
	case "normal":
		setFrameNormal()
	case "less":
		setFrameLess()
	case "fullscreen":
		setFullTopScreenMode()
	default:
		return
	}
}

//export goUserApiServer
func goUserApiServer() {
	if debug != 0 {
		logger.Println("enable api service")
	}
	http.HandleFunc("/win", func(w http.ResponseWriter, r *http.Request) {
		var upgrader = websocket.Upgrader{
			ReadBufferSize:  1024,
			WriteBufferSize: 1024,
			CheckOrigin:     func(r *http.Request) bool { return true },
		}
		conn, err := upgrader.Upgrade(w, r, nil)
		if err != nil {
			if debug != 0 {
				logger.Println("ws error :", err)
			}
			return
		}
		for {
			messageType, r, err := conn.NextReader()
			if err != nil {
				return
			}
			switch messageType {
			case websocket.TextMessage:
				bits := make([]byte, 1024)
				r.Read(bits)
				cmd := string(bits[:bytes.IndexByte(bits, 0)])
				switch {
				case cmd == "win:full":
					setFullWindow()
				case cmd == "win:topmost":
					setTopMost()
				case cmd == "win:nonetop":
					setNoneTop()
				case cmd == "win:close":
					setClose()
				case cmd == "win:max":
					setMaximize()
				case cmd == "win:min":
					setMinimize()
				case cmd == "win:normal":
					setFrameNormal()
				case cmd == "win:thin":
					setFrameThin()
				case cmd == "win:less":
					setFrameLess()
				case cmd == "win:fullscreen":
					setFullTopScreenMode();
				case cmd == "win:restore":
					setRestore()
				case strings.HasPrefix(cmd, "win:drag"):
					setBeginDrag()
				case strings.HasPrefix(cmd, "win:move|"):
					//if drag && (last == nil || last.Add(bottle).After(time.Now())) {
					ptr := strings.Split(cmd, "|")
					X, e1 := strconv.Atoi(ptr[1])
					Y, e2 := strconv.Atoi(ptr[2])
					if e1 != nil || e2 != nil {
						continue
					}
					if doMoveWindow(X, Y) {
						continue
					}
				case cmd == "win:drop":
					setEndDrag()
				}
			case websocket.CloseMessage:
				return
			default:
				data := make([]byte, 1024)
				r.Read(data)
				conn.WriteMessage(messageType, data)
			}
		}
	})
	service["win"] = true
}

//export goStartServer
func goStartServer(port *C.char) {
	if debug != 0 {
		logger.Println("called goStartServer")
	}
	apiSrv = &http.Server{Addr: C.GoString(port)}
	go WinSrv()
}
func WinSrv() {
	if debug != 0 {
		logger.Println("will start apiSrv ")
	}
	if len(service) == 0 {
		logger.Println("error no service enabled ")
		return
	}
	serverRunning = true

	if e := apiSrv.ListenAndServe(); e != nil {
		logger.Fatalln("start win api server failed", e)
		serverRunning = false
	}
}

func OffsetRect(rect *RECT, offX int32, offY int32) {
	rect.Top += offY
	rect.Bottom += offY
	rect.Left += offX
	rect.Right += offX
}
