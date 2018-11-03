package logger

import (
	"context"
	"os"
	"path/filepath"
	"strings"
	"sync"
	"time"
)

//<editor-fold desc="RotateWriter">
type RotateWriter struct {
	lock     sync.Mutex
	filename string // should be set to the actual filename
	fp       *os.File
	size     uint64
}

// Make a new RotateWriter. Return nil if error occurs during setup.
func NewRotateWriter(filename string, size uint64, rotateDelay time.Duration) (*RotateWriter, context.CancelFunc) {
	f, e := filepath.Abs(filename)
	if e != nil {
		panic(e)
		return nil, nil
	}
	w := &RotateWriter{filename: f, size: size, lock: sync.Mutex{}}
	err := w.Rotate()
	if err != nil {
		log.Errorf(`could not rotate file %+v`, err)
		return nil, nil
	}
	ctx, cancel := context.WithCancel(context.Background())
	go func() {
		defer w.fp.Sync()
		for {
			select {
			case <-ctx.Done():
				return
			default:
				err := w.Rotate()
				if err != nil {
					return
				}
				time.Sleep(rotateDelay)
			}
		}
	}()
	return w, cancel
}

// Write satisfies the io.Writer interface.
func (w *RotateWriter) Write(output []byte) (int, error) {
	w.lock.Lock()
	defer w.lock.Unlock()
	return w.fp.Write(output)
}

// Perform the actual act of rotating and reopening file.
func (w *RotateWriter) Rotate() (err error) {
	w.lock.Lock()
	defer w.lock.Unlock()
	// Close existing file if open
	if w.fp != nil {
		err = w.fp.Close()
		w.fp = nil
		if err != nil {
			return
		}
	}
	// Rename dest file if it already exists
	f, err := os.Stat(w.filename)
	if err == nil && (w.size == 0 || f.Size() >= int64(w.size)) {
		if err == nil {
			err = os.Rename(w.filename, w.filename+"."+strings.Replace(time.Now().Format(time.RFC3339), ":", "_", -1))
			if err != nil {
				return
			}
		}
		// Create a file.
		w.fp, err = os.Create(w.filename)
	} else if err != nil && (err == os.ErrNotExist ||
		strings.Contains(err.Error(), `no such file or directory`) ||
		strings.Contains(err.Error(), `system cannot find the file specified`)) {
		w.fp, err = os.Create(w.filename)
	} else if err != nil {
		panic(err)
	} else {
		w.fp, err = os.OpenFile(w.filename, os.O_WRONLY, 0666)
	}

	return
}

//</editor-fold>
