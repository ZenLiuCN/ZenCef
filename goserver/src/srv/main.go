package main

import (
	"logger"
	"os"
	"service"
	"time"
)

var (
	log *logger.Logger
)

func init() {
	w, _ := logger.NewRotateWriter(root()+`/logs/server.log`, logger.MustToBytes("1MB"), 15*time.Minute)
	logger.Init(logger.TRACE, os.Stdout, w)
	log = logger.GetLogger()
	service.SetLogger(log)

}
func main() {
	log.Infoln(`main started`)
	RegisterDBService()
	RegisterWinService()
	c := make(chan os.Signal, 1)
	service.ListenAndServe(`:8080`)
	defer service.Shutdown()
	<-c
	log.Debugln("main quited")
	os.Exit(0)
}
