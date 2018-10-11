package main

import (
	"errors"
	"github.com/gorilla/websocket"
)

var (
	ErrCmdErr     = errors.New("COMMAND format error")
	ErrFrameError = errors.New("COMMAND must start with frame id,eg: 1:CMD... ")
)

func wsText(ws *websocket.Conn, message string) error {
	return ws.WriteMessage(websocket.TextMessage, []byte(message))
}
func wsFText(ws *websocket.Conn, id string, message string) error {
	return ws.WriteMessage(websocket.TextMessage, []byte(id+":"+message))
}
func wsError(ws *websocket.Conn, message error) error {
	return ws.WriteMessage(websocket.TextMessage, []byte("ERROR|"+message.Error()))
}
func wsFError(ws *websocket.Conn, id string, message error) error {
	return ws.WriteMessage(websocket.TextMessage, []byte(id+":ERROR|"+message.Error()))
}
