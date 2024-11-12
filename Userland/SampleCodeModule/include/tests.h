#ifndef TEST_PROCESS_H
#define TEST_PROCESS_H

#include "./test_util.h"
#include "./syscall.h"

void test_mm(uint8_t argc, char * argv[]);
void test_processes(uint8_t argc, char *argv[]);
void test_prio(uint8_t argc, char *argv[]);
void test_sync(uint8_t argc, char * argv[]);

#endif