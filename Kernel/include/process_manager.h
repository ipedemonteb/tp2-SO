#ifndef _PROCESS_MANAGER_H
#define _PROCESS_MANAGER_H

#include <stdint.h>
#include "memory_manager.h"
#include "process.h"
#include "scheduler.h"
#include "lib.h"

#define QUANT		100
#define STACKSIZE   4096

typedef struct process_stack {
    uint8_t argc;
    uint8_t ** argv;
    void * rip;
    void * cs;
    void * rflags;
    void * ss;
} process_stack;

uint16_t createProcess(void * fn, uint8_t argc, uint8_t * argv[]);
uint8_t kill(uint16_t pid);
uint8_t block(uint16_t pid);
uint8_t unblock(uint16_t pid);
void nice(uint16_t pid, uint8_t priority);
void yield(); // es la que libera al CPU voluntariamente
void wait_children();

#endif