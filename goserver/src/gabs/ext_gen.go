// +build ignore

package main

import (
	"os"
	"strings"
	"text/template"
)

type Conf struct {
	TypeName string
	Types    []TypeDef
}
type TypeDef struct {
	Name         string
	Default      interface{}
	Convert      string
	Internal     string
	HasArray     bool
	OnlyArray    bool
	ConvertSlice bool
}

func main() {
	os.Remove("ext_container_generated.go")
	f, _ := os.OpenFile("ext_container_generated.go", os.O_CREATE|os.O_RDWR, os.ModePerm)
	funcs := map[string]interface{}{
		`upper`: func(a string) string {
			return strings.ToUpper(string([]rune(a)[0])) + string([]rune(a)[1:])
		},
		`notEmpty`: func(a string) bool { return a != "" },
	}
	tpl := template.Must(template.New("test").Funcs(funcs).Parse(TPL))

	tpl.Execute(f, Conf{
		TypeName: `Container`,
		Types: []TypeDef{
			{Name: `int`, Default: 0, Convert: `float64`, HasArray: true},
			{Name: `int8`, Default: 0, Convert: `float64`, HasArray: true},
			{Name: `int16`, Default: 0, Convert: `float64`, HasArray: true},
			{Name: `int32`, Default: 0, Convert: `float64`, HasArray: true},
			{Name: `int64`, Default: 0, Convert: `float64`, HasArray: true},
			{Name: `uint`, Default: 0, Convert: `float64`, HasArray: true},
			{Name: `uint8`, Default: 0, Convert: `float64`, HasArray: true},
			{Name: `uint16`, Default: 0, Convert: `float64`, HasArray: true},
			{Name: `uint32`, Default: 0, Convert: `float64`, HasArray: true},
			{Name: `uint64`, Default: 0, Convert: `float64`, HasArray: true},
			{Name: `float32`, Default: 0, Convert: `float64`, HasArray: true},
			{Name: `float64`, Default: 0, Convert: ``, HasArray: true},
			{Name: `string`, Default: "``", Convert: `[]byte`, HasArray: true},
			{Name: `byte`, Default: 0, Convert: `float64`, Internal: `uint8`, HasArray: true, ConvertSlice: true, OnlyArray: true},
			{Name: `bool`, Default: false, Convert: ``, HasArray: true},
		},
	})
}

const TPL = `// Code generated by "ext_gen.go ; DO NOT EDIT.
package gabs
import(
"fmt"
)
{{$name:=.TypeName}}{{range .Types}}
{{if not .OnlyArray}}
func (g *{{$name}}) MustGet{{upper .Name}}(path ...string)({{if .ConvertSlice}}[]{{.Name}}{{else}}{{.Name}}{{end}} ){
	if s,e:=g.Get{{upper .Name}}(path ...);e!=nil{panic(e)}else{return s}
}
func (g *{{$name}}) Get{{upper .Name}}(path ...string)({{if .ConvertSlice}}[]{{.Name}}{{else}}{{.Name}}{{end}},error ){
	{{if notEmpty .Internal}}if  r,e:=g.Get{{upper .Internal}}(path...);e==nil{ 	 	
 	 	return {{.Name}}(r),nil
 	 }{{end}}
    if g.Exists(path...) {
      if d, ok := g.S(path...).Data().({{.Name}}); ok {
        return d,nil
      }{{if notEmpty .Convert}}else if d,ok:=g.S(path...).Data().({{.Convert}});ok {
      		return {{if .ConvertSlice}}[]{{.Name}}{{else}}{{.Name}}{{end}}(d),nil
      	}{{end}}
      return {{.Default}},fmt.Errorf("Type Not Match: want {{.Name}}{{if notEmpty .Convert}}|{{.Convert}}{{end}} got %T ",g.S(path...).Data())
    }
    return {{.Default}},ErrInvalidPath
  }
func (g *{{$name}}) MustGet{{upper .Name}}P(path string)({{if .ConvertSlice}}[]{{.Name}}{{else}}{{.Name}}{{end}} ){
	if s,e:=g.Get{{upper .Name}}P(path);e!=nil{panic(e)}else{return s}
}
func (g *{{$name}}) Get{{upper .Name}}P(path string) ({{if .ConvertSlice}}[]{{.Name}}{{else}}{{.Name}}{{end}} ,error ) {
	{{if notEmpty .Internal}}if  r,e:=g.Get{{upper .Internal}}P(path);e==nil{
 	 	return {{.Name}}(r),nil
 	 }{{end}}
    if g.ExistsP(path) {
      if d, ok := g.Path(path).Data().({{.Name}}); ok {
        return d,nil
      }{{if notEmpty .Convert}}else if d,ok:=g.Path(path).Data().({{.Convert}}); ok {
      		return {{if .ConvertSlice}}[]{{.Name}}{{else}}{{.Name}}{{end}}(d),nil
      	}{{end}}
      return {{.Default}},fmt.Errorf("Type Not Match: want {{.Name}}|{{.Convert}} got %T ",g.Path(path).Data())
    }
    return {{.Default}},ErrInvalidPath
}{{end}}{{if .HasArray}}
func (g *{{$name}}) MustGet{{upper .Name}}Slice(path ...string)([]{{.Name}}){
	if s,e:=g.Get{{upper .Name}}Slice(path...);e!=nil{panic(e)}else{return s}
}
 func (g *{{$name}}) Get{{upper .Name}}Slice(path ...string) ([]{{.Name}} ,error){
 	 {{if notEmpty .Internal}}if  r,e:=g.Get{{upper .Internal}}Slice(path...);e==nil{
 	 	  rx := make([]{{.Name}}, len(r))
 	 	for i,r1:=range r{
 	 		rx[i]={{.Name}}(r1)
 	 	}
 	 	return rx,nil
 	 }{{end}}
      if g.Exists(path...) {
        if s, ok := g.S(path...).Data().([]interface{}); ok {
          if len(s) == 0 {
            return []{{.Name}}{},nil
          }
          {{if notEmpty .Convert}}var co=false{{end}}
          if _, ok := s[0].({{.Name}}); !ok {
             {{if notEmpty .Convert}}co=true{{else}} return nil,fmt.Errorf("Type Not Match: want {{.Name}} got %T ",s[0]) {{end}}
          }{{if notEmpty .Convert}}else if _,ok:= s[0].({{.Convert}}); co&&!ok {
             return nil,fmt.Errorf("Type Not Match: want {{.Name}}|{{.Convert}} got %T ",s[0]) 
          }{{end}}
          r := make([]{{.Name}}, len(s))
          for i, V := range s {
           {{if notEmpty .Convert}} if co{
            	 rs,ok := V.({{.Convert}})
            	 if !ok{
            	 	return nil,fmt.Errorf("Type Not Match: want {{.Name}}|{{.Convert}} got %T ",s[0]) 
            	 }
            	 r[i]={{.Name}}(rs)
            }else{ {{end}}
            	 r[i],ok = V.({{.Name}})
            	  if !ok{
              		return nil,fmt.Errorf("Type Not Match: want {{.Name}} got %T ",V)
            	 }
            {{if notEmpty .Convert}} } {{end}}  
          }
          return r,nil
        }
        return nil,ErrNotArray
      }
      return nil,ErrInvalidPath
    }
func (g *{{$name}}) MustGet{{upper .Name}}SliceP(path string)([]{{.Name}}){
	if s,e:=g.Get{{upper .Name}}SliceP(path);e!=nil{panic(e)}else{return s}
}
 func (g *{{$name}}) Get{{upper .Name}}SliceP(path string) ([]{{.Name}} ,error){
 	 	 {{if notEmpty .Internal}}if  r,e:=g.Get{{upper .Internal}}SliceP(path);e==nil{
 	 	  rx := make([]{{.Name}}, len(r))
 	 	for i,r1:=range r{
 	 		rx[i]={{.Name}}(r1)
 	 	}
 	 	return rx,nil
 	 }{{end}}
      if g.ExistsP(path) {
        if s, ok := g.Path(path).Data().([]interface{}); ok {
          if len(s) == 0 {
            return []{{.Name}}{},nil
          }
          {{if notEmpty .Convert}}var co=false{{end}}
          if _, ok := s[0].({{.Name}}); !ok {
             {{if notEmpty .Convert}}co=true{{else}} return nil,fmt.Errorf("Type Not Match: want {{.Name}} got %T ",s[0]) {{end}}
          }{{if notEmpty .Convert}}else if _,ok:= s[0].({{.Convert}}); co&&!ok {
             return nil,fmt.Errorf("Type Not Match: want {{.Name}}|{{.Convert}} got %T ",s[0]) 
          }{{end}}
          r := make([]{{.Name}}, len(s))
          for i, V := range s {
           {{if notEmpty .Convert}} if co{
            	 rs,ok := V.({{.Convert}})
            	 if !ok{
            	 	return nil,fmt.Errorf("Type Not Match: want {{.Name}}|{{.Convert}} got %T ",s[0]) 
            	 }
            	 r[i]={{.Name}}(rs)
            }else{ {{end}}
            	 r[i],ok = V.({{.Name}})
            	  if !ok{
              		return nil,fmt.Errorf("Type Not Match: want {{.Name}} got %T ",V)
            	 }
            {{if notEmpty .Convert}} } {{end}}  
          }
          return r,nil
        }
        return nil,ErrNotArray
      }
      return nil,ErrInvalidPath
    }
{{end}}{{end}}`