package gabs

import (
	"encoding/hex"
	"testing"
)

func TestParseMsgPack(t *testing.T) {
	a := map[string]interface{}{
		`a`: "123213",
		`b`: "1111111",
		`c`: []byte{0x32, 0x31, 0x5},
	}
	var b = make([]byte, 0, 1024)
	mencoder.ResetBytes(&b)
	mencoder.MustEncode(a)
	g, e := ParseMsgPack(b)
	t.Log(g, e, hex.Dump(b))
}
