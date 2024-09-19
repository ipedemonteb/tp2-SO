#ifndef SYSCALLER_H
#define SYSCALLER_H

#include <stdint.h>

#define UNUSED 0

uint32_t readCaller(uint8_t __unused, uint8_t *buffer, uint32_t size);
long int writeCaller(uint8_t __unused, uint8_t fd, const uint8_t *string, uint32_t size);
void printRectangleCaller(uint8_t __unused, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
uint64_t getScreenWidthCaller(uint8_t __unused);
uint64_t getScreenHeightCaller(uint8_t __unused);
void printCharCaller(uint8_t __unused, uint8_t c, uint32_t x, uint32_t y, uint32_t charColor, uint32_t bgColour);
void waitCaller(uint8_t __unused, int ticks);
uint8_t keyboardStatusCaller(uint8_t __unused);
uint8_t keyboardKeyCaller(uint8_t __unused);
void beepSysCaller(uint8_t __unused, uint32_t frequence, int waitTicks);
uint8_t fontSizeUpCaller(uint8_t __unused);
uint8_t fontSizeDownCaller(uint8_t __unused);
void getTimeCaller(uint8_t __unused, uint8_t bp[]);

#endif