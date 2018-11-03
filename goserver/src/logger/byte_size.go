package logger

import (
	"errors"
	"strconv"
	"strings"
	"unicode"
)

//<editor-fold desc="ByteSize">
type ByteSize uint64

const (
	//_           = iota // ignore first value by assigning to blank identifier
	B ByteSize = 1 << (10 * iota)
	KB
	MB
	GB
	TB
	PB
	EB
)

var invalidByteQuantityError = errors.New("byte quantity must be a positive integer with a unit of measurement like M, MB, MiB, G, GiB, or GB")

func (s ByteSize) String() string {
	switch {
	case s < KB:
		return "B"
	case s >= KB && s < MB:
		return "KB"
	case s >= MB && s < GB:
		return "MB"
	case s >= GB && s < TB:
		return "GB"
	case s >= TB && s < PB:
		return "TB"
	case s >= PB && s < EB:
		return "PB"
	default:
		return "EB"
	}
}
func (s ByteSize) Uint64() uint64 {
	return uint64(s)
}
func (s ByteSize) Int64() int64 {
	return int64(s)
}
func (s ByteSize) Float64() float64 {
	return float64(s)
}
func BytesToString(bytes uint64) string {
	unit := B
	switch {
	case bytes >= EB.Uint64():
		unit = EB
	case bytes >= PB.Uint64():
		unit = PB
	case bytes >= TB.Uint64():
		unit = TB
	case bytes >= GB.Uint64():
		unit = GB
	case bytes >= MB.Uint64():
		unit = MB
	case bytes >= KB.Uint64():
		unit = KB
	case bytes >= B.Uint64():
		unit = B
	case bytes == 0:
		return "0"
	}
	return strings.TrimSuffix(strconv.FormatFloat(float64(bytes)/unit.Float64(),
		'f', 1, 64),
		".0") + unit.String()
}
func ToBytes(s string) (uint64, error) {
	s = strings.TrimSpace(s)
	s = strings.ToUpper(s)
	i := strings.IndexFunc(s, unicode.IsLetter)
	if i == -1 {
		return 0, invalidByteQuantityError
	}
	bytesString, multiple := s[:i], s[i:]
	bytes, err := strconv.ParseFloat(bytesString, 64)
	if err != nil || bytes <= 0 {
		return 0, invalidByteQuantityError
	}
	switch multiple {
	case "E", "EB", "EIB":
		return uint64(bytes * EB.Float64()), nil
	case "P", "PB", "PIB":
		return uint64(bytes * PB.Float64()), nil
	case "T", "TB", "TIB":
		return uint64(bytes * TB.Float64()), nil
	case "G", "GB", "GIB":
		return uint64(bytes * GB.Float64()), nil
	case "M", "MB", "MIB":
		return uint64(bytes * MB.Float64()), nil
	case "K", "KB", "KIB":
		return uint64(bytes * KB.Float64()), nil
	case "B":
		return uint64(bytes), nil
	default:
		return 0, invalidByteQuantityError
	}
}
func MustToBytes(s string) uint64 {
	if r, e := ToBytes(s); e != nil {
		panic(e)
	} else {
		return r
	}
}

//</editor-fold>
