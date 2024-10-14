#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdint.h>

uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void * start, uint8_t value, uint32_t size);
int64_t satoi(uint8_t * str);
void bussy_wait(uint64_t n);
void endless_loop();
void endless_loop_print(uint64_t wait);
void numToStr(int num, uint8_t * str);
void print_process_info();

#endif