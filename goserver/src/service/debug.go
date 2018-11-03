package service

import (
	"fmt"
	mux2 "github.com/gorilla/mux"
)

func DumpMux() []string {
	a := make([]string, 0, 10)
	mux.Walk(func(route *mux2.Route, router *mux2.Router, ancestors []*mux2.Route) error {
		path, _ := route.GetPathRegexp()
		a = append(a, fmt.Sprintf("name:%s,path:%s,subs:%+v", route.GetName(), path, ancestors))
		return nil
	})
	return a
}
