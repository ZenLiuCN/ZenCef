package main

import (
	"archive/zip"
	"bufio"
	"fmt"
	"gabs"
	"io"
	"io/ioutil"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"syscall"
	"time"
)

const (
	versionURL = `http://192.168.99.100:801/update.json`
	appname    = `cef.exe`
)

func main() {
	http.DefaultClient.Timeout = 15 * time.Second
	r, e := http.Get(versionURL)
	if e != nil {
		return
	}
	defer r.Body.Close()
	b, e := ioutil.ReadAll(r.Body)
	if e != nil {
		return
	}
	g, e := gabs.ParseJSON(b)
	if e != nil {
		return
	}
	v, e := g.GetInt("version")
	if e != nil {
		return
	}
	uri, e := g.GetString("url")
	if e != nil {
		return
	}
	desc, e := g.GetString("desc")
	if e != nil {
		return
	}
	vr, e := ioutil.ReadFile(filepath.Join(root(), "version"))
	if e != nil && !os.IsNotExist(e) {
		return
	} else if e != nil && os.IsNotExist(e) {
		downLoad(uri, desc, -1, v)
		return
	}
	vg, e := gabs.ParseJSON(vr)
	if e != nil {
		downLoad(uri, desc, -1, v)
		return
	}
	vo, e := vg.GetInt("version")
	if e != nil {
		downLoad(uri, desc, -1, v)
		return
	}
	if vo < v {
		downLoad(uri, desc, vo, v)
	}

}
func root() string {
	f, e := filepath.Abs(os.Args[0])
	if e != nil {
		return "."
	}
	f = filepath.Dir(f)
	return f
}
func downLoad(uri, desc string, vo, vn int) {
	r := messageBox(`程序更新`, fmt.Sprintf("程序更新:\n有新版本程序<版本v %d>,当前版本<v %d>.\n\n%s\n\n\t\t\t是否更新? \n ", vn, vo, desc), MB_OKCANCEL|MB_ICONQUESTION|MB_SYSTEMMODAL|MB_SETFOREGROUND)
	if r != IDOK {
		return
	}
	ch := make(chan bool)
	go func() {
		ShowDialog(ch)
	}()
	defer DestroyWindow(procBox)
	SetWindowText(procText, `结束程序...`)
	//kill process
	cmd := exec.Command("cmd", "/c", "taskkill /F /T /IM "+appname)
	cmd.SysProcAttr = &syscall.SysProcAttr{HideWindow: true}
	cmd.Start()
	time.Sleep(1 * time.Second)
	cmd.Start()
	SetWindowText(procText, `开始下载...`)
	http.DefaultClient.Timeout = 15 * time.Second
	rs, e := http.Get(uri)
	if e != nil {
		return
	}
	defer rs.Body.Close()
	//f, e := os.OpenFile(filepath.Join(root(), appname), os.O_CREATE|os.O_TRUNC, 0666)
	f, e := ioutil.TempFile("", "update.")
	if e != nil {
		return
	}
	defer f.Close()
	bw := bufio.NewWriter(f)
	bw.ReadFrom(rs.Body)
	bw.Flush()
	SetWindowText(procText, `开始解压文件...`)
	_, e = Unzip(f.Name(), root())
	if e != nil {
		return
	}
	os.Remove(f.Name())
	g := gabs.New()
	g.Set(vn, "version")
	g.Set(time.Now(), "date")
	e = ioutil.WriteFile("version", g.Bytes(), 0666)
	if e != nil {
		return
	}
	SetWindowText(procText, `完成更新`)
	EnableWindow(procBtn, true)
	<-ch
	return
}

func Unzip(src string, dest string) ([]string, error) {

	var filenames []string

	r, err := zip.OpenReader(src)
	if err != nil {
		return filenames, err
	}
	defer r.Close()

	for _, f := range r.File {

		rc, err := f.Open()
		if err != nil {
			return filenames, err
		}
		defer rc.Close()

		// Store filename/path for returning and using later on
		fpath := filepath.Join(dest, f.Name)

		// Check for ZipSlip. More Info: http://bit.ly/2MsjAWE
		if !strings.HasPrefix(fpath, filepath.Clean(dest)+string(os.PathSeparator)) {
			return filenames, fmt.Errorf("%s: illegal file path", fpath)
		}

		filenames = append(filenames, fpath)
		if f.FileInfo().IsDir() {

			// Make Folder
			os.MkdirAll(fpath, os.ModePerm)

		} else {

			// Make File
			if err = os.MkdirAll(filepath.Dir(fpath), os.ModePerm); err != nil {
				return filenames, err
			}

			outFile, err := os.OpenFile(fpath, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, f.Mode())
			if err != nil {
				return filenames, err
			}

			_, err = io.Copy(outFile, rc)

			// Close the file without defer to close before next iteration of loop
			outFile.Close()

			if err != nil {
				return filenames, err
			}

		}
	}
	return filenames, nil
}
