package main
import "C"
import (
	"net/http"
	"os"
)

//export goUseHttpServer
func goUseHttpServer(root *C.char) int {
	dir := C.GoString(root)
	if debug != 0 {
		logger.Printf("will enable http service of %s ", dir)
	}
	if i, e := os.Stat(dir); e != nil {
		if debug != 0 {
			logger.Printf("check dir error: %s ", e)
		}
		return 0
	} else if !i.IsDir() {
		if debug != 0 {
			logger.Printf("check dir error: not dir of %s ", i)
		}
		return -1
	}
	if debug != 0 {
		logger.Printf("enable http service of %s ", dir)
	}
	http.Handle("/", http.FileServer(http.Dir(dir)))
	service["dir"] = true
	return 1
}
