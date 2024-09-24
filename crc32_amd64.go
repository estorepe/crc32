// crc32_amd64.go
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

func init() {
	if cpu.X86.HasAVX512F && cpu.X86.HasAVX512VL {
		useAVX512 = 1
	}
}

func updateAVX512(crc uint32, p []byte) uint32 {
	if len(p) > 0 {
		return uint32(C.crc32_avx512_update(C.uint32_t(crc), (*C.char)(unsafe.Pointer(&p[0])), C.size_t(len(p))))
	}
	return crc
}

func updateNEON(crc uint32, p []byte) uint32 {
	// Fallback to software implementation for AMD64 without AVX512
	return softwareUpdate(crc, p)
}

func softwareUpdate(crc uint32, p []byte) uint32 {
	// Implement a software fallback here
	// This is a simple implementation and not optimized
	table := makeTable()
	for _, v := range p {
		crc = table[byte(crc)^v] ^ (crc >> 8)
	}
	return crc
}

func makeTable() [256]uint32 {
	var table [256]uint32
	for i := 0; i < 256; i++ {
		crc := uint32(i)
		for j := 0; j < 8; j++ {
			if crc&1 == 1 {
				crc = (crc >> 1) ^ 0xEDB88320
			} else {
				crc >>= 1
			}
		}
		table[i] = crc
	}
	return table
}
