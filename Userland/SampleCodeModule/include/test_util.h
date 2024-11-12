#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdint.h>

int test();
uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void * start, uint8_t value, uint32_t size);
void * my_memset(void * destination, int32_t c, uint64_t length);
int64_t satoi(char * str);
void bussy_wait(uint64_t n);
void endless_loop(uint8_t argc, char ** argv);
void endless_loop_print(uint8_t wait, char ** argv);
void numToStr(int num, uint8_t * str);
void print_process_info();

#endif