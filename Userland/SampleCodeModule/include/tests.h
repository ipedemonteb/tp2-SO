#ifndef TEST_PROCESS_H
#define TEST_PROCESS_H

#include "./test_util.h"
#include "./syscall.h"
#include "./shell_graphics.h"

uint64_t test_mm(uint64_t argc, uint64_t max_memory);
int64_t test_processes(uint64_t argc, char *argv[]);
void test_prio();
uint64_t test_sync(uint64_t argc, char * argv[]);

#endif