#include <stdint.h>
#include <stdio.h>
#include "syscall.h"
#include "test_util.h"
#include "process_manager.h"
#include "sem.h"
#include "videoDriver.h"


uint64_t test_sync(uint64_t argc, char *argv[]);
uint64_t my_process_inc(uint64_t argc, char *argv[]);
