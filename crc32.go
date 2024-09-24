// crc32.go

package crc32

import (
	"hash"
)

const Size = 4

type digest struct {
	crc uint32
}

// New creates a new hash.Hash32 computing the CRC-32 checksum.
func New() hash.Hash32 {
	return &digest{0xFFFFFFFF}
}

// NewIEEE creates a new hash.Hash32 computing the CRC-32 checksum.
// It is included for compatibility with hash/crc32.
func NewIEEE() hash.Hash32 {
	return New()
}

func (d *digest) Sum32() uint32 {
	return d.crc ^ 0xFFFFFFFF
}

func (d *digest) Sum(in []byte) []byte {
	s := d.Sum32()
	return append(in, byte(s>>24), byte(s>>16), byte(s>>8), byte(s))
}

func (d *digest) Reset() {
	d.crc = 0xFFFFFFFF
}

func (d *digest) Size() int {
	return Size
}

func (d *digest) BlockSize() int {
	return 1
}

func (d *digest) Write(p []byte) (n int, err error) {
	if len(p) > 0 {
		d.crc = update(d.crc, p)
	}
	return len(p), nil
}

// Update returns the result of adding the bytes in p to the crc.
func Update(crc uint32, p []byte) uint32 {
	return update(crc, p)
}

// Checksum returns the CRC-32 checksum of data.
func Checksum(data []byte) uint32 {
	crc := uint32(0xFFFFFFFF)
	crc = Update(crc, data)
	return crc ^ 0xFFFFFFFF
}

// update is implemented in architecture-specific files
func update(crc uint32, p []byte) uint32
