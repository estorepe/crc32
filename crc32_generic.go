package crc32

func updateGeneric(crc uint32, p []byte) uint32 {
	tab := makeTable()
	for _, v := range p {
		crc = tab[byte(crc)^v] ^ (crc >> 8)
	}
	return crc
}

func makeTable() [256]uint32 {
	var tab [256]uint32
	for i := 0; i < 256; i++ {
		crc := uint32(i)
		for j := 0; j < 8; j++ {
			if crc&1 == 1 {
				crc = (crc >> 1) ^ 0xEDB88320
			} else {
				crc >>= 1
			}
		}
		tab[i] = crc
	}
	return tab
}
