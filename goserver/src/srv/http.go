package main

import (
	"net/http"
	"service"
)

func RegisterDirService(root string) {
	service.NewResourceService(`/`, http.StripPrefix("/", http.FileServer(http.Dir(root))))
}
