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
	rc            *RECT
	lastLBPos     *POINT
	win           HWND
	top           = false
	drag          = false
	mx            = GetSystemMetrics(SM_CXSCREEN)
	my            = GetSystemMetrics(SM_CXSCREEN)
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

//export goSetHwnd
func goSetHwnd(hwnd C.HWND) {
	if debug != 0 {
		if debug != 0 {
			logger.Println("called goSetHwnd")
		}
	}
	win = HWND(unsafe.Pointer(hwnd))
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
	if debug!=0{
		logger.Printf("will enable http service of %s ",dir)
	}
	if i, e := os.Stat(dir); e != nil {
		if debug!=0{
			logger.Printf("check dir error: %s ",e)
		}
		return 0
	} else if !i.IsDir() {
		if debug!=0{
			logger.Printf("check dir error: not dir of %s ",i)
		}
		return -1
	}
	if debug!=0{
		logger.Printf("enable http service of %s ",dir)
	}
	http.Handle("/", http.FileServer(http.Dir(dir)))
	service["dir"] = true
	return 1
}
//export goUserApiServer
func goUserApiServer()  {
	if debug!=0{
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
					MoveWindow(win, 0, 0, mx, my, true)
				case cmd == "win:topMost:true":
					rc = new(RECT)
					GetWindowRect(win, rc)
					top = true
					SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE)
				case cmd == "win:topMost:false":
					rc = new(RECT)
					GetWindowRect(win, rc)
					SetWindowPos(win, HWND_NOTOPMOST, rc.Left, rc.Top, rc.Right-rc.Left, rc.Bottom-rc.Top, SWP_SHOWWINDOW&^SWP_NOMOVE&^SWP_NOSIZE)
					top = false
				case cmd == "win:close":
					PostMessage(win, WM_SYSCOMMAND, SC_CLOSE, 0)
				case cmd == "win:max":
					PostMessage(win, WM_SYSCOMMAND, SC_MAXIMIZE, 0)
				case cmd == "win:min":
					PostMessage(win, WM_SYSCOMMAND, SC_MINIMIZE, 0)
				case cmd == "win:frame:0":
					SetWindowLong(win, GWL_STYLE, WS_OVERLAPPEDWINDOW|WS_THICKFRAME|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX)
					SetWindowPos(win, HWND(0), 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED)
					ShowWindow(win, SW_SHOW)
				case cmd == "win:frame:1":
					SetWindowLong(win, GWL_STYLE, GetWindowLong(win, GWL_STYLE) & ^WS_CAPTION|WS_THICKFRAME)
					SetWindowPos(win, HWND(0), 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED)
				case cmd == "win:frame:2":
					SetWindowLong(win, GWL_STYLE, WS_OVERLAPPED|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS)
					SetWindowPos(win, HWND(0), 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED)
				case cmd == "win:frame:3":
					SetWindowLong(win, GWL_STYLE, WS_OVERLAPPED|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS)
					rc = new(RECT)
					GetWindowRect(win, rc)
					top = true
					MoveWindow(win, 0, 0, mx, my, true)
					SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED)
				case cmd == "win:restore":
					if top && rc != nil {
						SetWindowPos(win, HWND_NOTOPMOST, rc.Left, rc.Top, rc.Right-rc.Left, rc.Bottom-rc.Top, SWP_SHOWWINDOW&^SWP_NOMOVE&^SWP_NOSIZE)
						top = false
					} else {
						PostMessage(win, WM_SYSCOMMAND, SC_RESTORE, 0)
					}
				case strings.HasPrefix(cmd, "win:drag:start"):
					drag = true
					lastLBPos = new(POINT)
					GetCursorPos(lastLBPos)
					//if debug!=0{logger.Println(lastLBPos)}
					//ScreenToClient(win, lastLBPos)
				case strings.HasPrefix(cmd, "win:drag:move|"):
					//if drag && (last == nil || last.Add(bottle).After(time.Now())) {
					if drag {
						ptr := strings.Split(cmd, "|")
						X, _ := strconv.Atoi(ptr[1])
						Y, _ := strconv.Atoi(ptr[2])
						ox := int32(X) - lastLBPos.X
						oy := int32(Y) - lastLBPos.Y
						tx := lastLBPos.X + ox
						ty := lastLBPos.Y + oy
						re := new(RECT)
						GetWindowRect(win, re)
						if ox != 0 || oy != 0 {
							OffsetRect(re, int32(ox), int32(oy))
							if tx > mx || ty > my || tx < 0 || ty < 0 {
								SetCursorPos(lastLBPos.X, lastLBPos.Y)
								continue
							}
							MoveWindow(win, re.Left, re.Top, re.Right-re.Left, re.Bottom-re.Top, false)
							lastLBPos.X = lastLBPos.X + ox
							lastLBPos.Y = lastLBPos.Y + oy
							SetCursorPos(lastLBPos.X, lastLBPos.Y)
						}
					}
				case cmd == "win:drag:stop":
					drag = false
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
