// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/lib.h"
#include <stdint.h>

void * memset(void * destination, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t)c;
	char * dst = (char *)destination;
	while(length--) {
		dst[length] = chr;
	}
	return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length){
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 && (uint64_t)source % sizeof(uint32_t) == 0 && length % sizeof(uint32_t) == 0) {
		uint32_t * d = (uint32_t *) destination;
		const uint32_t * s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++) {
			d[i] = s[i];
		}
	}
	else {
		uint8_t * d = (uint8_t *)destination;
		const uint8_t * s = (const uint8_t *)source;

		for (i = 0; i < length; i++) {
			d[i] = s[i];
		}
	}
	return destination;
}

uint64_t my_strlen(const char * string) {
	uint64_t i = 0;
	while(string[i] != 0) {
		i++;
	}
	return i;
}

void uint64_to_hex_str(uint64_t value, int8_t *buffer) {
    buffer[0] = '0';
    buffer[1] = 'x';

    int index = 2;
    int start = 0;
    

    for (int i = 15; i >= 0; i--) {
        uint8_t nibble = (value >> (i * 4)) & 0xF;
        
        if (nibble > 0 || start || i == 0) {
            start = 1;
            if (nibble < 10) {
                buffer[index++] = nibble + '0';
            } else {
                buffer[index++] = nibble - 10 + 'A';
            }
        }
    }

    buffer[index] = '\0';
}

uint8_t find_off_bit_128(uint64_t set1 , uint64_t set2){
	uint64_t aux = set1;
	uint8_t idx = 0;
	if (set1 > set2) {
		aux = set2;
		idx = 64;
	}
	
	while(aux % 2){
		aux /= 2;
		idx++;
	}
	return idx;
}

uint64_t set_n_bit_64(uint64_t set, uint8_t n){
	return set | ((uint64_t)1 << n);
}
uint64_t off_n_bit_64(uint64_t set, uint8_t n){
	return set & ~((uint64_t)1 << n);
}