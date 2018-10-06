package main

import "C"

//export goConsoleLogger
func goConsoleLogger(message *C.char, source *C.char, line int) {
	logger.Printf("\n[%s %d] %s\n", C.GoString(source), line, C.GoString(message))
}

//export goLog
func goLog(file *C.char, line C.int, funcs, format, message *C.char) {
	logger.Printf("\n%s:%d <%s>"+C.GoString(format), C.GoString(file), int(line), C.GoString(funcs), C.GoString(message))
}
