#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>

typedef enum FD
{
    STDOUT = 1,
    STDERR = 2
} FD;

uint32_t read(uint8_t *buffer, uint32_t size);                                                 // 0
long int write(FD fd, const uint8_t *string, uint32_t size);                                   // 1
void printRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t colors); // 2
uint64_t getScreenWidth();                                                                     // 3
uint64_t getScreenHeight();                                                                    // 4
void printChar(uint8_t c, uint32_t x, uint32_t y, uint32_t fgcolor, uint32_t bgcolor);         // 5
void wait(uint32_t ticks);                                                                     // 6
uint8_t keyboardStatus();                                                                      // 7
uint8_t keyboardKey();                                                                         // 8
void beepSyscall(uint32_t frequence, uint32_t waitTicks);                                      // 9
uint8_t fontSizeUp();                                                                          // 10
uint8_t fontSizeDown();                                                                        // 11
void getTime(uint8_t pb[]);                                                                    // 12

#endif