// crc32_arm64.go
package crc32

/*
#cgo CFLAGS: -march=armv8-a+crc+crypto
#include <stdint.h>
#include <stdlib.h>

uint32_t crc32_neon_update(uint32_t crc, const char* buf, size_t len);
*/
import "C"
import "unsafe"

func updateNEON(crc uint32, p []byte) uint32 {
	if len(p) > 0 {
		return uint32(C.crc32_neon_update(C.uint32_t(crc), (*C.char)(unsafe.Pointer(&p[0])), C.size_t(len(p))))
	}
	return crc
}

// func updateAVX512(crc uint32, p []byte) uint32 {
	// This function should never be called on ARM64
//	panic("AVX512 not supported on ARM64")
// }
