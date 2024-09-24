//go:build amd64
// +build amd64

package crc32

/*
#cgo CFLAGS: -mavx512f -mavx512vl -mclflushopt -mclwb -mpclmul -mcrc32
#include <stdint.h>
#include <stdlib.h>

uint32_t crc32_avx512_update(uint32_t crc, const char* buf, size_t len);
*/
import "C"
import (
	"unsafe"
	"golang.org/x/sys/cpu"
)

var useAVX512 bool

func init() {
	useAVX512 = cpu.X86.HasAVX512F && cpu.X86.HasAVX512VL
}

func update(crc uint32, p []byte) uint32 {
	if useAVX512 {
		return updateAVX512(crc, p)
	}
	return updateGeneric(crc, p)
}

func updateAVX512(crc uint32, p []byte) uint32 {
	if len(p) > 0 {
		return uint32(C.crc32_avx512_update(C.uint32_t(crc), (*C.char)(unsafe.Pointer(&p[0])), C.size_t(len(p))))
	}
	return crc
}
