package main

import "C"
import (
	"encoding/base64"
	"encoding/json"
	"fmt"
	"gabs"
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
)

var (
	localPool = make(map[string]*DB)
)

func getDBL(name string) *DB {
	return localPool[name]
}
func hasDBL(name string) bool {
	_, ok := localPool[name]
	return ok
}

//export goOpenDB
func goOpenDB(name, password *C.char) *C.char {
	dname := C.GoString(name)
	dpwd := C.GoString(password)
	if hasDBL(dname) {
		return C.CString(fmt.Sprintf(`{"status":false,"message":"%s"}`, `database already opened`))
	}
	db := new(DB)
	if er := db.Open(dname, dpwd); er != nil {
		return C.CString(JsonError(er))
	}
	localPool[dname] = db
	return C.CString(`{"status":true}`)
}

//export goOpened
func goOpened() *C.char {
	a := make([]string, 0, len(localPool))
	for key, _ := range localPool {
		a = append(a, key)
	}
	g, e := gabs.ParseObject(a)
	if e != nil {
		return C.CString(JsonError(e))
	}
	return C.CString(fmt.Sprintf(`{"status":true,"data":%s}`, g.String()))
}

//export goCloseDB
func goCloseDB(name *C.char) C.int {
	dname := C.GoString(name)
	if !hasDBL(dname) {
		return C.int(0)
	}
	delete(localPool, dname)
	return C.int(1)
}

//export goCloseAllDB
func goCloseAllDB() {
	localPool = make(map[string]*DB)
}

//export goQuerysDB
func goQuerysDB(name, query *C.char) *C.char {
	db := getDBL(C.GoString(name))
	if db == nil {
		return C.CString(`{"status":false,"message":"database not opened"}`)
	}
	querys := make([]string, 0, 2)
	e := json.Unmarshal([]byte(C.GoString(query)), &querys)
	if e != nil {
		return C.CString(JsonError(e))
	}
	g, e := db.Querys(querys)
	if e != nil {
		return C.CString(JsonError(e))
	}
	return C.CString(fmt.Sprintf(`{"status":true,"data":%s}`, g.String()))
}

//export goExecsDB
func goExecsDB(name, query *C.char) *C.char {
	db := getDBL(C.GoString(name))
	if db == nil {
		return C.CString(`{"status":false,"message":"database not opened"}`)
	}
	querys := make([]string, 0, 2)
	e := json.Unmarshal([]byte(C.GoString(query)), &querys)
	if e != nil {
		return C.CString(JsonError(e))
	}
	g, e := db.Execs(querys)
	if e != nil {
		return C.CString(JsonError(e))
	}
	return C.CString(fmt.Sprintf(`{"status":true,"data":%s}`, g.String()))
}
func JsonError(err error) string {
	b, e := json.Marshal(err.Error())
	if e != nil {
		return fmt.Sprintf(`{"status":false,"message":"%s"}`, strings.Replace(e.Error(), `\`, `\\`, -1))
	}
	return fmt.Sprintf(`{"status":false,"message":%s}`, string(b))
}

//export goDelDB
func goDelDB(name *C.char) C.int {
	var path string
	dname := C.GoString(name)
	if hasDBL(dname) {
		return C.int(-1)
	}
	path = filepath.Join(dbRoot, dname+".cache")
	er := os.Remove(path)
	if er != nil {
		return C.int(0)
	}
	return C.int(1)
}

//export goQueryDB
func goQueryDB(name, query *C.char) *C.char {
	db := getDBL(C.GoString(name))
	if db == nil {
		return C.CString(`{"status":false,"message":"database not opened"}`)
	}
	g, e := db.Query(C.GoString(query))
	if e != nil {
		return C.CString(JsonError(e))
	}
	return C.CString(fmt.Sprintf(`{"status":true,"data":%s}`, g.String()))
}

//export goExecDB
func goExecDB(name, query *C.char) *C.char {
	db := getDBL(C.GoString(name))
	if db == nil {
		return C.CString(`{"status":false,"message":"database not opened"}`)
	}
	g, e := db.Exec(C.GoString(query))
	if e != nil {
		return C.CString(JsonError(e))
	}
	return C.CString(fmt.Sprintf(`{"status":true,"data":%s}`, g.String()))
}

//export goExportDB
func goExportDB(name *C.char) *C.char {
	dname := C.GoString(name)
	if hasDBL(dname) {
		return C.CString(fmt.Sprintf(`{"status":false,"message":"%s"}`, `database is opened`))
	}
	path := filepath.Join(dbRoot, dname+".cache")
	b, e := ioutil.ReadFile(path)
	if e != nil && strings.Contains(e.Error(), `find the file specified.`) {
		return C.CString(fmt.Sprintf(`{"status":false,"message":"%s"}`, `database not exists`))
	} else if e != nil {
		return C.CString(JsonError(e))
	}
	return C.CString(fmt.Sprintf(`{"status":true,"data":"%s"}`, base64.StdEncoding.EncodeToString(b)))
}
