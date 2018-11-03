package main

import "C"
import "logger"

//export goConsoleLogger
func goConsoleLogger(message *C.char, source *C.char, line int) {
	log.Printf(logger.INFO, "\n[%s %d] %s\n", C.GoString(source), line, C.GoString(message))
}

//export goLog
func goLog(file *C.char, line C.int, funcs, format, message *C.char) {
	log.Printf(logger.INFO, "\n%s:%d <%s>"+C.GoString(format), C.GoString(file), int(line), C.GoString(funcs), C.GoString(message))
}
