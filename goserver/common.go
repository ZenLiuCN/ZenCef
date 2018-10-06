package main

import "C"
import (
	"log"
	"os"
)

var (
	logger = log.New(os.Stdout, "[GO]", log.LstdFlags|log.Llongfile)
	debug  = 0
)
//export goSetDebug
func goSetDebug(i C.int) {
	debug = int(i)
}
func main() {
	if debug != 0 {
		logger.Println("main quited")
	}
	os.Exit(0)
}
