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
	dbPool   = make(map[net.Addr]map[string]*DB)
)

func init() {
	dbRoot = filepath.Join(root(), "cache")+string(os.PathSeparator)
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
	/*if !strings.HasPrefix(q, "INSERT") || !strings.HasPrefix(q, "UPDATE") || !strings.HasPrefix(q, "DELETE") {
		er = ErrNotExec
		return
	}*/
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
	/*if !strings.HasPrefix(q, "SELECT") {
		er = ErrNotQuery
		return
	}*/
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

func hasDB(name string) bool {
	for _, maps := range dbPool {
		if maps == nil || len(maps) == 0 {
			continue
		} else {
			for name, _ := range maps {
				if name == name {
					return true
				}
			}
		}
	}
	return false
}
func getDB(addr net.Addr, name string) *DB {
	if pool, ok := dbPool[addr]; !ok {
		return nil
	} else if db, ok := pool[name]; ok {
		return db
	}
	return nil
}
func CloseAll(addr net.Addr) {
	for _, value := range dbPool[addr] {
		value.Close()
	}
	delete(dbPool, addr)
}

func Open(addr net.Addr, name, password string) error {
	if getDB(addr, name) != nil {
		return ErrAlreadyOpen
	}
	if hasDB(name) {
		return ErrOtherOpen
	}
	db := new(DB)
	if er := db.Open(name, password); er != nil {
		return er
	}
	if _, ok := dbPool[addr]; !ok {
		dbPool[addr] = make(map[string]*DB)
	}
	dbPool[addr][name] = db
	return nil
}
func Query(addr net.Addr, name, query string) (r string, e error) {
	db := getDB(addr, name)
	if db == nil {
		e = ErrNotOpen
		return
	}
	return db.Query(query)
}
func Exec(addr net.Addr, name, query string) (r string, e error) {
	db := getDB(addr, name)
	if db == nil {
		e = ErrNotOpen
		return
	}
	return db.Exec(query)
}
func Close(addr net.Addr, name string) (er error) {
	db := getDB(addr, name)
	if db == nil {
		return ErrNotOpen
	}
	er = db.Close()
	if er != nil {
		return
	}
	delete(dbPool[addr], name)
	return
}
func Delete(addr net.Addr, name string) (er error) {
	var path string
	if hasDB(name) {
		return ErrOtherOpen
	}
	if d := getDB(addr, name); d != nil {
		er = d.Close()
		if er != nil {
			return
		}
		path = d.Path
		delete(dbPool[addr], name)
	} else {
		path = filepath.Join(dbRoot, name+".cache")
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
				cmds := strings.Split(cmd, "|")
				if len(cmds) < 2 {
					wsError(conn, ErrFrameError)
					continue
				}
				id := cmds[0]
				cmd = cmds[1]
				switch {
				case cmd == "OPEN":
					if len(cmds) != 4 {
						wsFError(conn, id, ErrCmdErr)
						continue
					}
					er := Open(conn.RemoteAddr(), cmds[2], cmds[3])
					if er != nil {
						wsFError(conn, id, er)
						continue
					}
					wsFText(conn, id, "SUCCESS")
				case cmd == "EXEC":
					if len(cmds) != 4 {
						wsFError(conn, id, ErrCmdErr)
						continue
					}
					r, er := Exec(conn.RemoteAddr(), cmds[2], cmds[3])
					if er != nil {
						wsFError(conn, id, er)
						continue
					}
					wsFText(conn, id, r)
				case cmd == "QUERY":
					if len(cmds) != 4 {
						wsFError(conn, id, ErrCmdErr)
						continue
					}
					r, er := Exec(conn.RemoteAddr(), cmds[2], cmds[3])
					if er != nil {
						wsFError(conn, id, er)
						continue
					}
					wsFText(conn, id, r)
				case cmd == "CLOSE":
					if len(cmds) != 3 {
						wsFError(conn, id, ErrCmdErr)
						continue
					}
					er := Close(conn.RemoteAddr(), cmds[2])
					if er != nil {
						wsFError(conn, id, er)
						continue
					}
					wsFText(conn, id, "SUCCESS")
				case cmd == "DELETE":
					if len(cmds) != 3 {
						wsFError(conn, id, ErrCmdErr)
						continue
					}
					er := Delete(conn.RemoteAddr(), cmds[2])
					if er != nil {
						wsFError(conn, id, er)
						continue
					}
					wsFText(conn, id, "SUCCESS")
				}
			case websocket.CloseMessage:
				CloseAll(conn.RemoteAddr())
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
