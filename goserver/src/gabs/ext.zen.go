package gabs

import (
	`encoding/json`
	"github.com/ugorji/go/codec"
	`io`
	`io/ioutil`
	`reflect`
)

//<editor-fold desc="MsgPack Extension">
var (
	mhandler = new(codec.MsgpackHandle)
	mdecoder = codec.NewDecoderBytes(nil, mhandler)
	mencoder = codec.NewEncoderBytes(nil, mhandler)
)

func init() {
	mhandler.RawToString = true
}

func ParseMsgPack(sample []byte) (*Container, error) {
	var gabs Container
	var err error
	if gabs.object, err = parseMsgPackToInterface(sample); err != nil {
		return nil, err
	}
	return &gabs, nil
}
func parseMsgPackToInterface(data []byte) (interface{}, error) {
	mdecoder.ResetBytes(data)
	if data[0] == 0xdf || data[0] == 0xde || (data[0] >= 0x80 && data[0] <= 0x8f) {
		a := map[string]interface{}{}
		e := mdecoder.Decode(&a)
		return a, e
	} else if data[0] == 0xdc || data[0] == 0xdd || (data[0] >= 0x90 && data[0] <= 0x9f) {
		var a []interface{}
		e := mdecoder.Decode(&a)
		return a, e
	} else {
		var a interface{}
		e := mdecoder.Decode(&a)
		return a, e
	}
}
func ParseMsgPackDecoder(decoder *codec.Decoder) (*Container, error) {
	var gabs Container
	if err := decoder.Decode(&gabs.object); err != nil {
		return nil, err
	}
	return &gabs, nil
}
func ParseMsgPackFile(path string) (*Container, error) {
	if len(path) > 0 {
		cBytes, err := ioutil.ReadFile(path)
		if err != nil {
			return nil, err
		}

		container, err := ParseMsgPack(cBytes)
		if err != nil {
			return nil, err
		}

		return container, nil
	}
	return nil, ErrInvalidPath
}
func ParseMsgPackBuffer(buffer io.Reader) (*Container, error) {
	var gabs Container
	jsonDecoder := codec.NewDecoder(buffer, mhandler)
	if err := jsonDecoder.Decode(&gabs.object); err != nil {
		return nil, err
	}

	return &gabs, nil
}
func (g *Container) MsgPackBytes() []byte {
	if g.Data() != nil {
		bytes := make([]byte, 0, 1024)
		mencoder.ResetBytes(&bytes)
		if err := mencoder.Encode(g.object); err == nil {
			return bytes
		}
	}
	return []byte("{}")
}

//</editor-fold>
func (g *Container) IsArray(path ...string) bool {
	if g.Exists(path...) {
		if _, ok := g.S(path...).Data().([]interface{}); ok {
			return true
		}
	}
	return false
}
func (g *Container) IsMap(path ...string) bool {
	if g.Exists(path...) {
		if _, ok := g.S(path...).Data().(map[string]interface{}); ok {
			return true
		}
	}
	return false
}
func (g *Container) IsNil(path ...string) bool {
	if g.Exists(path...) {
		return false
	}
	return true
}
func (g *Container) ReflectType(path ...string) string {
	if g.Exists(path...) {
		return reflect.TypeOf(g.S(path...).Data()).Kind().String()
	}
	return reflect.Kind(0).String()
}


func (g *Container) ContainsPath(path ...string) bool {
	for _, v := range path {
		if !g.ExistsP(v){
			return false
		}
	}
	return true
}

func ParseObject(data interface{}) (*Container, error)   {
	d,e:=json.Marshal(data)
	if e!=nil{
		return nil,e
	}
	return ParseJSON(d)
}
func MustParseObject(data interface{})(r *Container)  {
	d,e:=json.Marshal(data)
	if e!=nil{
		panic(e)
	}
	r,e= ParseJSON(d)
	if e!=nil{
		panic(e)
	}
	return r
}
func MustParseJSON(data []byte)( *Container) {
	r,e:=ParseJSON(data)
	if e!=nil{
		panic(e)
	}
	return r
}