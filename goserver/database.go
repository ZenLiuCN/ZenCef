package main

import "C"
import (
	"bytes"
	"database/sql"
	"encoding/json"
	"errors"
	"github.com/gorilla/websocket"
	_ "github.com/mattn/go-sqlite3"
	"net"
	"net/http"
	"os"
	"path/filepath"
	"strings"
)

var (
	ErrNotOpen     = errors.New("no such database opened,use OPEN first")
	ErrNotQuery    = errors.New("query should use QUERY")
	ErrNotExec     = errors.New("execute should use EXEC")
	ErrAlreadyOpen = errors.New("database already opened")
	ErrOtherOpen   = errors.New("database already opened by other session")
)
var (
	dbRoot   = "file:./cache/"
	dataRoot = "file:./cache/"
	dbPool   = make(map[string]*DB)
	wsPool   = make(map[net.Addr]string)
)

func init() {
	dbRoot = filepath.Join(root(), "cache")
	if _, err := os.Stat(dbRoot); os.IsNotExist(err) {
		os.MkdirAll(dbRoot, os.ModePerm)
	}
	dataRoot = "file:" + dbRoot
}

type DB struct {
	Name string
	Path string
	db   *sql.DB
}

func (d *DB) Open(name, password string) (er error) {
	d.db, er = sql.Open("sqlite3", dataRoot+name+".cache?_auth&_auth_user="+name+"&_auth_pass="+password)
	d.Name = name
	d.Path = filepath.Join(dbRoot, name+".cache")
	if er != nil {
		return
	}
	return nil
}
func (d *DB) Exec(q string) (res string, er error) {
	if d.db == nil {
		er = ErrNotOpen
		return
	}
	if !strings.HasPrefix(q, "INSERT") || !strings.HasPrefix(q, "UPDATE") || !strings.HasPrefix(q, "DELETE") {
		er = ErrNotExec
		return
	}
	r, e := d.db.Exec(q)
	if e != nil {
		er = e
		return
	}
	var data []byte
	data, er = json.Marshal(r)
	if er != nil {
		return
	}
	res = string(data)
	return
}
func (d *DB) Query(q string) (res string, er error) {
	if d.db == nil {
		er = ErrNotOpen
		return
	}
	if !strings.HasPrefix(q, "SELECT") {
		er = ErrNotQuery
		return
	}
	var rs *sql.Rows
	rs, er = d.db.Query(q)
	if er != nil {
		return
	}
	var r []map[string]interface{}
	rs.Scan(&r)
	rs.Close()
	var data []byte
	data, er = json.Marshal(r)
	if er != nil {
		return
	}
	res = string(data)
	return
}
func (d *DB) Close() (er error) {
	if d.db == nil {
		return
	}
	return d.db.Close()
}

func root() string {
	f, e := filepath.Abs(os.Args[0])
	if e != nil {
		return "."
	}
	f = filepath.Dir(f)
	return f
}

func Open(addr net.Addr, name, password string) error {
	if _, ok := wsPool[addr]; ok {
		return ErrOtherOpen
	}
	if _, ok := dbPool[name]; ok {
		return ErrAlreadyOpen
	}
	db := new(DB)
	if er := db.Open(name, password); er != nil {
		return er
	}
	dbPool[name] = db
	wsPool[addr] = name
	return nil
}
func Query(addr net.Addr, query string) (r string, e error) {
	var (
		db string
		ok bool
	)
	if db, ok = wsPool[addr]; !ok {
		e = ErrNotOpen
		return
	}
	if d, ok := dbPool[db]; !ok {
		e = ErrNotOpen
		return
	} else {
		return d.Query(query)
	}
}
func Exec(addr net.Addr, query string) (r string, e error) {
	var (
		db string
		ok bool
	)
	if db, ok = wsPool[addr]; !ok {
		e = ErrNotOpen
		return
	}
	if d, ok := dbPool[db]; !ok {
		e = ErrNotOpen
		return
	} else {
		return d.Exec(query)
	}
}
func Close(addr net.Addr, name string) (er error) {
	if db, ok := dbPool[name]; ok {
		er = db.Close()
	}
	if er != nil {
		return
	}
	delete(dbPool, name)
	delete(wsPool, addr)
	return
}
func Delete(addr net.Addr, db string) (er error) {
	var path string
	if d, ok := dbPool[db]; ok {
		d.Close()
		path = d.Path
		delete(dbPool, db)
		delete(wsPool, addr)
	} else {
		path = filepath.Join(dbRoot, db+".cache")
	}
	er = os.Remove(path)
	return
}

//export goUseDBServer
func goUseDBServer() {
	if debug != 0 {
		logger.Println("enable db service")
	}
	http.HandleFunc("/db", func(w http.ResponseWriter, r *http.Request) {
		var up = websocket.Upgrader{
			ReadBufferSize:  1024,
			WriteBufferSize: 1024,
			CheckOrigin:     func(r *http.Request) bool { return true },
		}
		conn, err := up.Upgrade(w, r, nil)
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
				bits := make([]byte, 4096)
				r.Read(bits)
				cmd := string(bits[:bytes.IndexByte(bits, 0)])
				frames := strings.Split(cmd, ":")
				if len(frames) != 2 {
					wsError(conn, ErrFrameError)
					continue
				}
				id := frames[0]
				cmd = frames[1]
				switch {
				case strings.HasPrefix(cmd, "OPEN|"):
					ptr := strings.Split(cmd, "|")
					if len(ptr) != 3 {
						wsFError(conn, id, ErrCmdErr)
						continue
					}
					er := Open(conn.RemoteAddr(), ptr[1], ptr[2])
					if er != nil {
						wsFError(conn, id, er)
						continue
					}
					wsFText(conn, id, "SUCCESS")
				case strings.HasPrefix(cmd, "EXEC|"):
					ptr := strings.Split(cmd, "|")
					if len(ptr) != 2 {
						wsFError(conn, id, ErrCmdErr)
						continue
					}
					r, er := Exec(conn.RemoteAddr(), ptr[1])
					if er != nil {
						wsFError(conn, id, er)
						continue
					}
					wsFText(conn, id, r)
				case strings.HasPrefix(cmd, "QUERY|"):
					ptr := strings.Split(cmd, "|")
					if len(ptr) != 2 {
						wsFError(conn, id, ErrCmdErr)
						continue
					}
					r, er := Exec(conn.RemoteAddr(), ptr[1])
					if er != nil {
						wsFError(conn, id, er)
						continue
					}
					wsFText(conn, id, r)
				case strings.HasPrefix(cmd, "DELETE|"):
					ptr := strings.Split(cmd, "|")
					if len(ptr) != 2 {
						wsFError(conn, id, ErrCmdErr)
						continue
					}
					er := Delete(conn.RemoteAddr(), ptr[1])
					if er != nil {
						wsFError(conn, id, er)
						continue
					}
					wsFText(conn, id, "SUCCESS")
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
	service["db"] = true
}
