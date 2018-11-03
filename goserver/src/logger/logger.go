package logger

import (
	"fmt"
	"io"
	LOG "log"
	"os"
)

var (
	log *Logger
)

func Init(level LogLevel, writer ...io.Writer) {
	if len(writer) == 0 {
		log = NewLogger(os.Stdout, level)
	} else {
		w := NewWriter(writer...)
		log = NewLogger(w, level)
	}
	log.Escape = 2

}

func GetLogger() *Logger {
	return log
}
func Println(lvl LogLevel, args ...interface{}) {
	log.Escape += 1
	log.Println(lvl, args...)
	log.Escape -= 1
}
func Printf(lvl LogLevel, format string, args ...interface{}) {
	log.Escape += 1
	log.Printf(lvl, format, args...)
	log.Escape -= 1
}
func Fatalln(args ...interface{}) {
	log.Escape += 1
	log.Println(FATAL, args...)
	log.Escape -= 1
}
func Fatalf(format string, args ...interface{}) {
	log.Escape = 3
	log.Printf(FATAL, format, args...)
	log.Escape = 2
}
func Errorln(args ...interface{}) {
	log.Escape += 1
	log.Println(ERROR, args...)
	log.Escape -= 1
}
func Errorf(format string, args ...interface{}) {
	log.Escape += 1
	log.Printf(ERROR, format, args...)
	log.Escape -= 1
}
func Warnln(args ...interface{}) {
	log.Escape += 1
	log.Println(WARN, args...)
	log.Escape -= 1
}
func Warnf(format string, args ...interface{}) {
	log.Escape += 1
	log.Printf(WARN, format, args...)
	log.Escape -= 1
}
func Debugln(args ...interface{}) {
	log.Escape += 1
	log.Println(DEBUG, args...)
	log.Escape -= 1
}
func Debugf(format string, args ...interface{}) {
	log.Escape += 1
	log.Printf(DEBUG, format, args...)
	log.Escape -= 1
}
func Infoln(args ...interface{}) {
	log.Escape += 1
	log.Println(INFO, args...)
	log.Escape -= 1
}
func Infof(format string, args ...interface{}) {
	log.Escape += 1
	log.Printf(INFO, format, args...)
	log.Escape -= 1
}
func Traceln(args ...interface{}) {
	log.Escape += 1
	log.Println(TRACE, args...)
	log.Escape -= 1
}
func Tracef(format string, args ...interface{}) {
	log.Escape += 1
	log.Printf(TRACE, format, args...)
	log.Escape -= 1
}

//<editor-fold desc="logger">
type LogLevel int

const (
	TRACE LogLevel = iota
	DEBUG
	INFO
	WARN
	ERROR
	FATAL
)

func ParseLogLevel(i int) LogLevel {
	if i >= 0 && i <= 5 {
		return LogLevel(i)
	}
	return TRACE
}
func (l LogLevel) String() string {
	switch l {
	case TRACE:
		return "TRACE"
	case DEBUG:
		return "DEBUG"
	case INFO:
		return "INFO"
	case WARN:
		return "WARN"
	case ERROR:
		return "ERROR"
	case FATAL:
		return "FATAL"
	default:
		return "NONE"
	}
}

type Logger struct {
	l      *LOG.Logger
	Escape int
	Level  LogLevel
}

func (l Logger) CurrentLevel() int {
	return int(l.Level)
}
func (l Logger) Println(lvl LogLevel, args ...interface{}) {
	if lvl < l.Level {
		return
	}
	l.l.SetPrefix(`[` + lvl.String() + `] `)
	l.l.Output(l.Escape, fmt.Sprintln(args...))
	if lvl == FATAL {
		os.Exit(1)
	}
}
func (l Logger) Printf(lvl LogLevel, format string, args ...interface{}) {
	if lvl < l.Level {
		return
	}
	l.l.SetPrefix(`[` + lvl.String() + `] `)
	l.l.Output(l.Escape, fmt.Sprintf(format, args...))
	if lvl == FATAL {
		os.Exit(1)
	}
}
func (l Logger) Fatalln(args ...interface{}) {
	l.Escape += 1
	l.Println(FATAL, args...)
	l.Escape -= 1
}
func (l Logger) Fatalf(format string, args ...interface{}) {
	l.Escape += 1
	l.Printf(FATAL, format, args...)
	l.Escape -= 1
}
func (l Logger) Errorln(args ...interface{}) {
	l.Escape += 1
	l.Println(ERROR, args...)
	l.Escape -= 1
}
func (l Logger) Errorf(format string, args ...interface{}) {
	l.Escape += 1
	l.Printf(ERROR, format, args...)
	l.Escape -= 1
}
func (l Logger) Warnln(args ...interface{}) {
	l.Escape += 1
	l.Println(WARN, args...)
	l.Escape -= 1
}
func (l Logger) Warnf(format string, args ...interface{}) {
	l.Escape += 1
	l.Printf(WARN, format, args...)
	l.Escape -= 1
}
func (l Logger) Infoln(args ...interface{}) {
	l.Escape += 1
	l.Println(INFO, args...)
	l.Escape -= 1
}
func (l Logger) Infof(format string, args ...interface{}) {
	l.Escape += 1
	l.Printf(INFO, format, args...)
	l.Escape -= 1
}
func (l Logger) Debugln(args ...interface{}) {
	l.Escape += 1
	l.Println(DEBUG, args...)
	l.Escape -= 1
}
func (l Logger) Debugf(format string, args ...interface{}) {
	l.Escape += 1
	l.Printf(DEBUG, format, args...)
	l.Escape -= 1
}
func (l Logger) Traceln(args ...interface{}) {
	l.Escape += 1
	l.Println(TRACE, args...)
	l.Escape -= 1
}
func (l Logger) Tracef(format string, args ...interface{}) {
	l.Escape += 1
	l.Printf(TRACE, format, args...)
	l.Escape -= 1
}
func NewLogger(o io.Writer, level LogLevel) *Logger {
	l := new(Logger)
	l.Level = level
	l.l = LOG.New(o, "[INFO]", LOG.LstdFlags|LOG.Lshortfile)
	return l
}

//</editor-fold>
