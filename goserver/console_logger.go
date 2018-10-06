package main
import "C"

//export goConsoleLogger
func goConsoleLogger(message *C.char, source *C.char, line int) {
	logger.Printf("\n[%s %d] %s\n", C.GoString(source), line, C.GoString(message))
}