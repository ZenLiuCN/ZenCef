package main

/*
#include <windows.h>
*/
import "C"
import (
	. "github.com/lxn/win"
	"net"
	"os"
	"service"
	"strconv"
	"strings"
	"syscall"
	"unsafe"
)

//export goStopServer
func goStopServer() {
	service.Shutdown()
}

//export goGetExtJson
func goGetExtJson() *C.char {
	return C.CString(`var WinObj;WinObj||(WinObj={});(function(){WinObj.wss="ws://127.0.0.1:65530/win";WinObj.close=function(){WinObj.WinWS.send("win:close")};WinObj.full=function(){WinObj.WinWS.send("win:full")};WinObj.topMost=function(){WinObj.WinWS.send("win:topMost")};WinObj.max=function(){WinObj.WinWS.send("win:max")};WinObj.min=function(){WinObj.WinWS.send("win:min")};WinObj.restore=function(){WinObj.WinWS.send("win:restore")};WinObj.drag=function(a){a?WinObj.WinWS.send("win:drag:start"):WinObj.WinWS.send("win:drag:stop")};WinObj.onMouseMove=function(a){eve=window.event||ev;WinObj.WinWS.send("win:drag:move|"+eve.offsetX+"|"+eve.offsetY)}})();`)
}

//export goIsServerStarted
func goIsServerStarted() int {
	if service.IsRunning() {
		return 1
	} else {
		return 0
	}
}

//export goRunSchemeCommand
func goRunSchemeCommand(url *C.char) {
	uri := C.GoString(url)
	act := strings.Split(strings.Replace(strings.ToLower(uri), "window://", "", -1), "/")

	if len(act) == 0 {
		log.Errorf("scheme command of  %s  is empty\n", uri)
		return
	}
	if win == HWND(0) {
		log.Errorf("scheme command %s not set window handler of %p \n", uri, unsafe.Pointer(win))
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

//export goStartServer
func goStartServer(port *C.char) C.int {
	addr := C.GoString(port)
	ln, err := net.Listen("tcp", addr)
	if err != nil {
		log.Errorf("Can't listen on port %q: %s", addr, err)
		m, _ := syscall.UTF16PtrFromString("65530端口被占用,无法启动系统.\n请检查是否已经启动本系统!")
		c, _ := syscall.UTF16PtrFromString("错误")
		MessageBox(HWND(0), m, c, MB_OK|MB_ICONERROR)
		return C.int(0)
	}
	ln.Close()
	service.ListenAndServe(addr)
	return C.int(1)
}

//export goSetHwnd
func goSetHwnd(hwnd unsafe.Pointer) {
	log.Debugln("called goSetHwnd")
	win = HWND(unsafe.Pointer(hwnd))
}

//export goUseWinServer
func goUseWinServer() {
	log.Debugln("enable win service")
	RegisterWinService()
}

//export goUseHttpServer
func goUseHttpServer(root *C.char) int {
	dir := C.GoString(root)
	log.Debugf("will enable http service of %s ", dir)
	if i, e := os.Stat(dir); e != nil {
		log.Errorf("check dir error: %s ", e)
		return 0
	} else if !i.IsDir() {
		log.Errorf("check dir error: not dir of %s ", i)
		return -1
	}
	log.Debugf("enable http service of %s ", dir)
	RegisterDirService(dir)
	return 1
}

//export goUseDBServer
func goUseDBServer() {
	log.Debugln("enable db service")
	RegisterDBService()
}
