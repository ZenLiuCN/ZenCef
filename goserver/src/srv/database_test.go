package main

import (
	"gabs"
	"testing"
	"time"
	"ws"
)

func TestDB_Exec(t *testing.T) {
	db := new(DB)
	t.Log(db.Open("123", "123"))
	t.Log(db.Query(`select * from foo`))
	t.Log(db.Query(`pragma table_info ('FOO')`))
	t.Log(db.Exec(`delete from foo`))
}
func TestWebsocket(t *testing.T) {
	w := new(ws.WsSrv)
	w.Dial("ws://127.0.0.1:8080/db", func(id string, msg *gabs.Container) (*gabs.Container, error) {
		t.Logf(`get response %s`, msg.String())
		return nil, nil
	})
	time.Sleep(5 * time.Second)
	g := gabs.New()
	g.Set(ws.GetMessageId(), `id`)
	g.Set(`open`, `cmd`)
	g.Set(`open`, `name`)
	g.Set(`open`, `pwd`)
	t.Log(w.Send(g))
	time.Sleep(2 * time.Second)
	g = gabs.New()
	g.Set(ws.GetMessageId(), `id`)
	g.Set(`query`, `cmd`)
	g.Set(`open`, `name`)
	g.Set(`create table foo(id INT,name TEXT,createAt DATE)`, `query`)
	t.Log(w.Send(g))
	time.Sleep(2 * time.Second)
}
