package service

type Logger interface {
	CurrentLevel() int
	Fatalln(args ...interface{})
	Fatalf(format string, args ...interface{})
	Errorln(args ...interface{})
	Errorf(format string, args ...interface{})
	Warnln(args ...interface{})
	Warnf(format string, args ...interface{})
	Infoln(args ...interface{})
	Infof(format string, args ...interface{})
	Debugln(args ...interface{})
	Debugf(format string, args ...interface{})
	Traceln(args ...interface{})
	Tracef(format string, args ...interface{})
}

var (
	log Logger
)

func SetLogger(logger Logger) {
	log = logger
}
