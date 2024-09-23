#ifndef _LIBC_H_
#define _LIBC_H_

#include <stdint.h>

#define EOF -1
#define NULL 0

int printf(const uint8_t * fmt, ...);
int putChar(uint8_t c);
int getChar();
void cleanBuffer();
void scanf(const uint8_t * fmt, ...);
int atoi(const uint8_t * str);
void itos(int num, uint8_t * buffer);
int strcmp(uint8_t * s1, uint8_t * s2);

#endif