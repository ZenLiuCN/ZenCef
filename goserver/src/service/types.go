package service

import (
	"io"
)

type WsHandlerFunction func(conn *WsConnJson, err error, messageType int, message io.Reader) error
