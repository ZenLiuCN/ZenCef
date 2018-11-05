package main

import (
	"database/sql"
	"encoding/json"
	"errors"
	"gabs"
	_ "github.com/mattn/go-sqlite3"
	"net"
	"os"
	"path/filepath"
	"service"
	"strings"
)

var (
	ErrNotOpen     = errors.New("no such database opened,use OPEN first")
	ErrCmdFormat   = errors.New("command format error")
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
	dbRoot = filepath.Join(root(), "cache") + string(os.PathSeparator)
	if _, err := os.Stat(dbRoot); os.IsNotExist(err) {
		os.MkdirAll(dbRoot, os.ModePerm)
	}
	dataRoot = "file:" + dbRoot

}

type DB struct {
	Name string
	Path string
	pwd  string
	db   *sql.DB
}

func (d *DB) open() (er error) {
	d.db, er = sql.Open("sqlite3", d.Path+"?_auth&_auth_user="+d.Name+"&_auth_pass="+d.pwd)
	return er
}

func (d *DB) Open(name, password string) (er error) {
	d.Name = name
	d.pwd = password
	d.Path = filepath.Join(dbRoot, name+".cache")
	er = d.open()
	if er != nil {
		return
	}
	defer d.db.Close()
	return nil
}
func (d *DB) Exec(q string) (rs *gabs.Container, er error) {
	er = d.open()
	if er != nil {
		return
	}
	defer d.db.Close()
	r, e := d.db.Exec(q)
	if e != nil {
		er = e
		return
	}
	rs = gabs.New()
	if id, e := r.LastInsertId(); e == nil {
		rs.Set(id, `LastInsertId`)
	}
	if id, e := r.RowsAffected(); e == nil {
		rs.Set(id, `RowsAffected`)
	}
	return
}
func (d *DB) Query(q string) (res *gabs.Container, er error) {
	er = d.open()
	if er != nil {
		return
	}
	defer d.db.Close()
	var rs *sql.Rows
	rs, er = d.db.Query(q)
	if er != nil {
		return
	}
	defer rs.Close()
	data, er := mapResult(rs)
	if er != nil {
		return
	}
	return gabs.ParseJSON(data)
}
func (d *DB) ExecParam(q string,p ...interface{}) (rs *gabs.Container, er error) {
	er = d.open()
	if er != nil {
		return
	}
	defer d.db.Close()
	r, e := d.db.Exec(q,p...)
	if e != nil {
		er = e
		return
	}
	rs = gabs.New()
	if id, e := r.LastInsertId(); e == nil {
		rs.Set(id, `LastInsertId`)
	}
	if id, e := r.RowsAffected(); e == nil {
		rs.Set(id, `RowsAffected`)
	}
	return
}
func (d *DB) QueryParam(q string,p ...interface{}) (res *gabs.Container, er error) {
	er = d.open()
	if er != nil {
		return
	}
	defer d.db.Close()
	var rs *sql.Rows
	rs, er = d.db.Query(q,p...)
	if er != nil {
		return
	}
	defer rs.Close()
	data, er := mapResult(rs)
	if er != nil {
		return
	}
	return gabs.ParseJSON(data)
}
func (d *DB) Querys(query []string) (res *gabs.Container, er error) {
	er = d.open()
	if er != nil {
		return
	}
	defer d.db.Close()
	tx, er := d.db.Begin()
	if er != nil {
		return
	}
	rss := make([]interface{}, 0, len(query))
	for _, v := range query {
		var rs *sql.Rows
		rs, er = tx.Query(v)
		if er != nil {
			return
		}
		data, err := mapResult(rs)
		if err != nil {
			rs.Close()
			er = err
			return
		}
		rs.Close()
		var re *gabs.Container
		re, er = gabs.ParseJSON(data)
		if er != nil {
			return
		}
		rss = append(rss, re.Data())
	}
	er = tx.Commit()
	if er != nil {
		return
	}
	return gabs.Consume(rss)
}
func (d *DB) Execs(q []string) (res *gabs.Container, er error) {
	er = d.open()
	if er != nil {
		return
	}
	defer d.db.Close()
	tx, er := d.db.Begin()
	if er != nil {
		return
	}
	rss := make([]interface{}, 0, len(q))
	for _, v := range q {
		r, e := tx.Exec(v)
		if e != nil {
			er = e
			return
		}
		rs := map[string]interface{}{}
		if id, e := r.LastInsertId(); e == nil {
			rs[`LastInsertId`] = id
		}
		if id, e := r.RowsAffected(); e == nil {
			rs[`RowsAffected`] = id
		}
		rss = append(rss, rs)
	}
	er = tx.Commit()
	if er != nil {
		return
	}
	return gabs.Consume(rss)
	return
}
func mapResult(r *sql.Rows) ([]byte, error) {
	cols := make([]interface{}, 0, 10)
	colType, _ := r.ColumnTypes()
	for _, v := range colType {
		log.Tracef("col %s %s", v.Name(), v.DatabaseTypeName())
	}
	for r.Next() {
		/*		colData := make([]interface{}, len(colType))
				colPtr := make([]interface{}, len(colType))
				for i, v := range colData {
					colPtr[i] = &v
				}
				if e := r.Scan(colPtr...); e != nil {
					log.Errorf(`error scan data %v`, e)
					return nil, e
				}
				data := make(map[string]interface{})
				for idx, tp := range colType {
					logger.Tracef(`col %s %T %+v`,tp.Name(),colPtr[idx],colPtr[idx])
					v:=colPtr[idx].(*interface{})
					switch tp.DatabaseTypeName(){
					case `TEXT`:	data[tp.Name()] =string( (*v).([]byte))
					default:
						data[tp.Name()] = *v
					}
				}*/
		columns := make([]interface{}, len(colType))
		columnPointers := make([]interface{}, len(colType))
		for i, _ := range columns {
			columnPointers[i] = &columns[i]
		}
		if err := r.Scan(columnPointers...); err != nil {
			return nil, err
		}
		m := make(map[string]interface{})
		for i, cType := range colType {
			val := columnPointers[i].(*interface{})
			switch strings.ToUpper(cType.DatabaseTypeName()) {
			case `TEXT`:
				m[cType.Name()] = string((*val).([]byte))
			case ``: //pragma
				switch cType.Name() {
				case `name`, `type`:
					m[cType.Name()] = string((*val).([]byte))
				default:
					m[cType.Name()] = *val
				}
			default:
				m[cType.Name()] = *val
			}
		}
		cols = append(cols, m)
	}
	return json.Marshal(cols)

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
	} else if pool != nil {
		if db, ok := pool[name]; ok {
			return db
		}
	}
	return nil
}
func CloseAll(addr net.Addr) {
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
func Query(addr net.Addr, name, query string) (res *gabs.Container, e error) {
	db := getDB(addr, name)
	if db == nil {
		e = ErrNotOpen
		return
	}
	return db.Query(query)
}
func Querys(addr net.Addr, name string, query []string) (res *gabs.Container, e error) {
	db := getDB(addr, name)
	if db == nil {
		e = ErrNotOpen
		return
	}
	return db.Querys(query)
}
func Exec(addr net.Addr, name, query string) (res *gabs.Container, e error) {
	db := getDB(addr, name)
	if db == nil {
		e = ErrNotOpen
		return
	}
	return db.Exec(query)
}
func Execs(addr net.Addr, name string, query []string) (res *gabs.Container, e error) {
	db := getDB(addr, name)
	if db == nil {
		e = ErrNotOpen
		return
	}
	return db.Execs(query)
}
func Close(addr net.Addr, name string) (er error) {
	delete(dbPool[addr], name)
	return
}
func Delete(addr net.Addr, name string) (er error) {
	var path string
	if hasDB(name) {
		return ErrOtherOpen
	}
	delete(dbPool[addr], name)
	path = filepath.Join(dbRoot, name+".cache")
	er = os.Remove(path)
	return
}

func RegisterDBService() {
	service.NewWebsocketService(`database`, `/db`, service.WsGabsCommander(databaseCommander), 1024, 1024)
}

//return none nil will close websocket connection
func databaseCommander(conn *service.WsConnJson, mType int, id string, m *gabs.Container) error {
	switch strings.ToLower(m.GetString(`cmd`)) {
	case `open`:
		name := m.GetString(`name`)
		pwd := m.GetString(`pwd`)
		if name == "" || pwd == "" {
			conn.SendErrorResponse(id, ErrCmdFormat)
			return nil
		}
		if e := Open(conn.RemoteAddr(), name, pwd); e != nil {
			conn.SendErrorResponse(id, e)
			return nil
		}
		conn.SendResponseSuccess(id)
		return nil
	case `close`:
		name := m.GetString(`name`)
		if name == "" {
			conn.SendErrorResponse(id, ErrCmdFormat)
			return nil
		}
		if e := Close(conn.RemoteAddr(), name); e != nil {
			conn.SendErrorResponse(id, e)
			return nil
		}
		conn.SendResponseSuccess(id)
		return nil
	case `delete`:
		name := m.GetString(`name`)
		if name == "" {
			conn.SendErrorResponse(id, ErrCmdFormat)
			return nil
		}
		if e := Delete(conn.RemoteAddr(), name); e != nil {
			conn.SendErrorResponse(id, e)
			return nil
		}
		conn.SendResponseSuccess(id)
		return nil
	case `exec`:
		name := m.GetString(`name`)
		query := m.GetString(`query`)
		if name == "" {
			conn.SendErrorResponse(id, ErrCmdFormat)
			return nil
		}
		if query == "" && !m.IsArray(`querys`) {
			conn.SendErrorResponse(id, ErrCmdFormat)
			return nil
		}
		if query != "" {
			if r, e := Query(conn.RemoteAddr(), name, query); e != nil {
				conn.SendErrorResponse(id, e)
				return nil
			} else {
				g := gabs.New()
				g.Set(r.Data(), `data`)
				g.Set(true, `status`)
				conn.SendResponse(id, g)
			}
		} else {
			qs := m.GetStrings(`querys`)
			if r, e := Execs(conn.RemoteAddr(), name, qs); e != nil {
				conn.SendErrorResponse(id, e)
				return nil
			} else {
				g := gabs.New()
				g.Set(r.Data(), `data`)
				g.Set(true, `status`)
				conn.SendResponse(id, g)
			}
		}
		return nil
	case `query`:
		name := m.GetString(`name`)
		query := m.GetString(`query`)
		if name == "" {
			conn.SendErrorResponse(id, ErrCmdFormat)
			return nil
		}
		if query == "" && !m.IsArray(`querys`) {
			conn.SendErrorResponse(id, ErrCmdFormat)
			return nil
		}
		if query != "" {
			if r, e := Query(conn.RemoteAddr(), name, query); e != nil {
				conn.SendErrorResponse(id, e)
				return nil
			} else {
				g := gabs.New()
				g.Set(r.Data(), `data`)
				g.Set(true, `status`)
				conn.SendResponse(id, g)
			}
		} else {
			qs := m.GetStrings(`querys`)
			if r, e := Querys(conn.RemoteAddr(), name, qs); e != nil {
				conn.SendErrorResponse(id, e)
				return nil
			} else {
				g := gabs.New()
				g.Set(r.Data(), `data`)
				g.Set(true, `status`)
				conn.SendResponse(id, g)
			}
		}

	}
	return nil
}
