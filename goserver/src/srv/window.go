package main

import "C"
import (
	"bytes"
	"github.com/gorilla/websocket"
	. "github.com/lxn/win"
	"io"
	"service"
	"strconv"
	"strings"
)

var (
	rc        *RECT
	lastLBPos *POINT
	win       HWND
	top       = false
	drag      = false
	mx        = GetSystemMetrics(SM_CXSCREEN)
	my        = GetSystemMetrics(SM_CYSCREEN)
)

func setTopMost() {
	rc = new(RECT)
	GetWindowRect(win, rc)
	top = true
	SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE)
}
func setClose() {
	PostMessage(win, WM_SYSCOMMAND, SC_CLOSE, 0)
}
func setFullWindow() {
	MoveWindow(win, 0, 0, mx, my, true)
}
func setNoneTop() {
	rc = new(RECT)
	GetWindowRect(win, rc)
	SetWindowPos(win, HWND_NOTOPMOST, rc.Left, rc.Top, rc.Right-rc.Left, rc.Bottom-rc.Top, SWP_SHOWWINDOW&^SWP_NOMOVE&^SWP_NOSIZE)
	top = false
}
func setMaximize() {
	PostMessage(win, WM_SYSCOMMAND, SC_MAXIMIZE, 0)
}
func setMinimize() {
	PostMessage(win, WM_SYSCOMMAND, SC_MINIMIZE, 0)
}
func setFrameNormal() {
	SetWindowLong(win, GWL_STYLE, WS_OVERLAPPEDWINDOW|WS_THICKFRAME|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX)
	SetWindowPos(win, HWND(0), 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED)
	ShowWindow(win, SW_SHOW)
}
func setFrameThin() {
	SetWindowLong(win, GWL_STYLE, GetWindowLong(win, GWL_STYLE) & ^WS_CAPTION | WS_THICKFRAME)
	SetWindowPos(win, HWND(0), 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED)
}
func setFrameLess() {
	SetWindowLong(win, GWL_STYLE, WS_OVERLAPPED|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS)
	SetWindowPos(win, HWND(0), 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED)
}
func setFullTopScreenMode() {
	SetWindowLong(win, GWL_STYLE, WS_OVERLAPPED|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS)
	rc = new(RECT)
	GetWindowRect(win, rc)
	top = true
	MoveWindow(win, 0, 0, mx, my, true)
	SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED)
}
func setRestore() {
	if top && rc != nil {
		SetWindowPos(win, HWND_NOTOPMOST, rc.Left, rc.Top, rc.Right-rc.Left, rc.Bottom-rc.Top, SWP_SHOWWINDOW&^SWP_NOMOVE&^SWP_NOSIZE)
		top = false
	} else {
		PostMessage(win, WM_SYSCOMMAND, SC_RESTORE, 0)
	}
}
func setBeginDrag() {
	drag = true
	lastLBPos = new(POINT)
	GetCursorPos(lastLBPos)
}
func doMoveWindow(X, Y int) bool {
	if drag {
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
				return true
			}
			MoveWindow(win, re.Left, re.Top, re.Right-re.Left, re.Bottom-re.Top, false)
			lastLBPos.X = lastLBPos.X + ox
			lastLBPos.Y = lastLBPos.Y + oy
			SetCursorPos(lastLBPos.X, lastLBPos.Y)
		}
	}
	return false
}
func setEndDrag() {
	drag = false
}

func OffsetRect(rect *RECT, offX int32, offY int32) {
	rect.Top += offY
	rect.Bottom += offY
	rect.Left += offX
	rect.Right += offX
}

func RegisterWinService() {
	service.NewWebsocketService(`win`, `/win`, func(conn *service.WsConnJson, err error, messageType int, message io.Reader) error {
		switch messageType {
		case -1:
			if err != nil {
				log.Errorf("ws upgrade error %+v", err)
				return err
			}
		case 0:
			return nil
		case websocket.TextMessage:
			bits := make([]byte, 1024)
			message.Read(bits)
			cmd := string(bits[:bytes.IndexByte(bits, 0)])
			switch {
			case cmd == "full":
				setFullWindow()
			case cmd == "topmost":
				setTopMost()
			case cmd == "nonetop":
				setNoneTop()
			case cmd == "close":
				setClose()
			case cmd == "max":
				setMaximize()
			case cmd == "min":
				setMinimize()
			case cmd == "normal":
				setFrameNormal()
			case cmd == "thin":
				setFrameThin()
			case cmd == "less":
				setFrameLess()
			case cmd == "fullscreen":
				setFullTopScreenMode()
			case cmd == "restore":
				setRestore()
			case strings.HasPrefix(cmd, "drag"):
				setBeginDrag()
			case strings.HasPrefix(cmd, "move|"):
				//if drag && (last == nil || last.Add(bottle).After(time.Now())) {
				ptr := strings.Split(cmd, "|")
				X, e1 := strconv.Atoi(ptr[1])
				Y, e2 := strconv.Atoi(ptr[2])
				if e1 != nil || e2 != nil {
					return nil
				}
				if doMoveWindow(X, Y) {
					return nil
				}
			case cmd == "drop":
				setEndDrag()
			}

		}
		return nil
	}, 1024, 1024)
}
