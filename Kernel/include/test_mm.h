#ifndef TEST_MM_H
#define TEST_MM_H

#include "test_util.h"
#include "memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

uint64_t test_mm(uint64_t argc, uint64_t max_memory);

uint64_t test_mm1(uint64_t argc, uint8_t * argv[]);

#endif