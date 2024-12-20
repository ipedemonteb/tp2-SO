#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void *memset(void *destination, int32_t character, uint64_t length);
void *memcpy(void *destination, const void *source, uint64_t length);

uint64_t my_strlen(const char *string);
void uint64_to_hex_str(uint64_t value, int8_t *buffer);

void halt_cpu();
char *cpuVendor(char *result);

void outb(uint8_t X, uint8_t Y);
uint8_t inb(uint8_t X);
void int20();
uint8_t find_off_bit_64(uint64_t set);
uint8_t find_off_bit_128(uint64_t set1, uint64_t set2);
uint64_t set_n_bit_64(uint64_t set, uint8_t n);
uint64_t off_n_bit_64(uint64_t set, uint8_t n);

#endif