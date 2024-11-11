#ifndef _LIBC_H_
#define _LIBC_H_

#include <stdint.h>

#define EOF -1
#define NULL 0

int printf(const char * fmt, ...);
int putChar(uint8_t c);
int64_t put_char(char c);
int getChar();
char get_char();
void cleanBuffer();
void scanf(const uint8_t * fmt, ...);
int atoi(char * str);
void itos(int num, char * buffer);
int strcmp(char * s1, char * s2);
uint32_t strcpy(char * dest, const char * source);
int64_t char_idx(const char *s, char c);
char * strtok(char * string, const char * delim);
uint32_t strlen(const char * str);
void uint64ToHexStr(uint64_t value, char *buffer);
#endif