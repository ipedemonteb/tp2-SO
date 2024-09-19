#ifndef _LIBC_H_
#define _LIBC_H_
#define EOF -1
#define NULL 0

int printf(const char *fmt, ...);
int putChar(char c);
int getChar();
void cleanBuffer();
void scanf(const char *fmt, ...);
int atoi(const char *str);
void itos(int num, char *buffer);
int strcmp(char *s1, char *s2);

#endif