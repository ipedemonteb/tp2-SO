#ifndef _KEYBOARD_DRIVER_H
#define _KEYBOARD_DRIVER_H
#include <stdint.h>

#define RIGHTKEY 0x4D
#define LEFTKEY 0x4B
#define UPKEY 0x48
#define DOWNKEY 0x50

#define DELETE 127
#define LEFT_ARROW 251
#define RIGHT_ARROW 252
#define UP_ARROW 253
#define DOWN_ARROW 254
#define FIRST_BREAK_CODE 128
#define ESC_MAKE_CODE 0x01
#define ESC_BREAK_CODE 0x81
#define R_MAKE_CODE 0x13
#define R_BREAK_CODE 0x93
#define ENTER_B 0x9C
#define CAPS_LOCK 0x3A

#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define LEFT_CTRL 0x1D

#define LEFT_SHIFT_B 0xAA
#define RIGHT_SHIFT_B 0xB6
#define LEFT_CTRL_B 0x9D

#define BUFF_SIZE 512

uint8_t keyHandler();

void keyboard_handler();

uint8_t nextFromBuffer();
uint8_t getKey(); // pulling
uint8_t getKeyboardStatus();
uint8_t getKeyboardKey(); // pullingn't

#endif