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
	server    *http.Server
	rc        *RECT
	lastLBPos *POINT
	win       HWND
	top       = false
	drag      = false
	mx        = GetSystemMetrics(SM_CXSCREEN)
	my        = GetSystemMetrics(SM_CXSCREEN)
)

//export goStopServer
func goStopServer() {
	println("called goStopServer")
	server.Close()
}

//export goSetHwnd
func goSetHwnd(hwnd C.HWND) {
	println("called goSetHwnd")
	win = HWND(unsafe.Pointer(hwnd))
}

//export goGetExtJson
func goGetExtJson() *C.char {
	println("called goGetExtJson")
	return C.CString(`var WinObj;WinObj||(WinObj={});(function(){WinObj.wss="ws://127.0.0.1:65530/win";WinObj.close=function(){WinObj.WinWS.send("win:close")};WinObj.full=function(){WinObj.WinWS.send("win:full")};WinObj.topMost=function(){WinObj.WinWS.send("win:topMost")};WinObj.max=function(){WinObj.WinWS.send("win:max")};WinObj.min=function(){WinObj.WinWS.send("win:min")};WinObj.restore=function(){WinObj.WinWS.send("win:restore")};WinObj.drag=function(a){a?WinObj.WinWS.send("win:drag:start"):WinObj.WinWS.send("win:drag:stop")};WinObj.onMouseMove=function(a){eve=window.event||ev;WinObj.WinWS.send("win:drag:move|"+eve.offsetX+"|"+eve.offsetY)}})();`)
}

//export goStartServer
func goStartServer() {
	println("called goStartServer")
	go WinSrv()
}
func WinSrv() {
	println("will start server ")
	server = &http.Server{Addr: ":65530"}
	http.HandleFunc("/win", func(w http.ResponseWriter, r *http.Request) {
		var upgrader = websocket.Upgrader{
			ReadBufferSize:  1024,
			WriteBufferSize: 1024,
			CheckOrigin:     func(r *http.Request) bool { return true },
		}
		conn, err := upgrader.Upgrade(w, r, nil)
		if err != nil {
			println("ws error :", err)
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
					SetWindowLong(win, GWL_STYLE, GetWindowLong(win, GWL_STYLE) & ^WS_CAPTION | WS_THICKFRAME)
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
					//logger.Println(lastLBPos)
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
	server.ListenAndServe()
}
func main() {
	println("main quited")
	os.Exit(0)
}
func OffsetRect(rect *RECT, offX int32, offY int32) {
	rect.Top += offY
	rect.Bottom += offY
	rect.Left += offX
	rect.Right += offX
}
