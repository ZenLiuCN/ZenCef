package main

import (
	"os"
	"path/filepath"
)

func root() string {
	f, e := filepath.Abs(os.Args[0])
	if e != nil {
		return "."
	}
	f = filepath.Dir(f)
	return f
}
