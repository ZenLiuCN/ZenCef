package service

import (
	"gabs"
	"io"
)

func WsGabsCommander(commander func(conn *WsConnJson, msgType int, id string, msg *gabs.Container) error) func(conn *WsConnJson, err error, messageType int, message io.Reader) error {
	return func(conn *WsConnJson, err error, messageType int, message io.Reader) error {
		switch messageType {
		case -1:
			if err != nil {
				log.Errorf("ws upgrade error %+v", err)
				return err
			}
		case 0:
			return nil
		default:
			g, e := gabs.ParseJSONBuffer(message)
			if e != nil {
				log.Errorf(`read data error %v`, e)
				conn.SendError(e)
				return nil
			}
			if e := conn.HandleCallback(g); e != nil && e != ErrNoCallback {
				return e
			} else if e == nil {
				return nil
			}
			id ,_:= g.GetString(`id`)
			return commander(conn, messageType, id, g)
		}
		return nil
	}
}