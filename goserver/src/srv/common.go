package main

import "C"
import (
	"logger"
)

//export goSetDebug
func goSetDebug(i C.int) {
	log.Level = logger.ParseLogLevel(int(i))
}
