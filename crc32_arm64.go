//go:build arm64
// +build arm64

package crc32

/*
#cgo CFLAGS: -march=armv8-a+crc+crypto
#include <stdint.h>
#include <stdlib.h>

uint32_t crc32_neon_update(uint32_t crc, const char* buf, size_t len);
*/
import "C"
import "unsafe"

func update(crc uint32, p []byte) uint32 {
	return updateNEON(crc, p)
}

func updateNEON(crc uint32, p []byte) uint32 {
	if len(p) > 0 {
		return uint32(C.crc32_neon_update(C.uint32_t(crc), (*C.char)(unsafe.Pointer(&p[0])), C.size_t(len(p))))
	}
	return crc
}
