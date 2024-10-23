#ifndef TEST_PROCESS_H
#define TEST_PROCESS_H

#include "test_util.h"
#include "syscall.h"

int64_t test_processes(uint64_t argc, char *argv[]);

void test_prio();

#endif