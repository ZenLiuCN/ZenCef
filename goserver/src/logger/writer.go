package logger

import (
	"io"
)

type LoggerWriter struct {
	Writers []io.Writer
}

func (w *LoggerWriter) Write(output []byte) (int, error) {
	for _, v := range w.Writers {
		if i, e := v.Write(output); e != nil {
			return i, e
		}
	}
	return len(output), nil
}
func NewWriter(w ...io.Writer) io.Writer {
	writer := new(LoggerWriter)
	writer.Writers = append([]io.Writer{}, w...)
	return writer
}
