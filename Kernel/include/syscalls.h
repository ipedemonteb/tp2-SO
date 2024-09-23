#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>
typedef enum FD {
    STDOUT = 1,
    STDERR = 2
} FD;

uint32_t read(uint8_t * buffer, uint32_t size);
long int write(FD fd, uint8_t * string, uint32_t size);
void printRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t colors);
uint64_t getScreenWidth();
uint64_t getScreenHeight();
void printChar(uint8_t c, uint32_t x, uint32_t y, uint32_t fgcolor, uint32_t bgcolor);
void wait(uint32_t ticks);
uint8_t keyboardStatus();
uint8_t keyboardKey();
void beepSyscall(uint32_t frequence, uint32_t waitTicks);
uint8_t fontSizeUp();
uint8_t fontSizeDown();
void getTime(uint8_t pb[]);

#endif