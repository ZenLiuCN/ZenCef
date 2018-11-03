package logger

import (
	"os"
	"testing"
	"time"
)

func TestNewLogger(t *testing.T) {
	w, c := NewRotateWriter("log.log", MustToBytes("1KB"), time.Second)
	defer c()
	X := NewWriter(w, os.Stdout)
	log := NewLogger(X, TRACE)
	log.Infof("info test")
	log.Infof("info test1")
	log.Infof("info test2")
	log.Infof("info test3")
}
