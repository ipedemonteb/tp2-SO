#ifndef _PROCESS_MANAGER_H
#define _PROCESS_MANAGER_H

#include <stdint.h>
#include "memory_manager.h"
#include "process.h"
#include "scheduler.h"
#include "lib.h"

#define QUANT		129

typedef struct process_stack {
    void * r15;
	void * r14;
	void * r13;
	void * r12;
	void * r11;
	void * r10;
	void * r9;
	void * r8;
	void * rsi;
	void * rdi;
	void * rbp;
	void * rdx;
	void * rcx;
	void * rbx;
	void * rax;
    void * rip;
    void * cs;
    void * rflags;
    void * rsp;
    void * ss;
} process_stack;

int32_t create_process(void (*fn)(uint8_t, uint8_t **), uint8_t argc, uint8_t * argv[]);
uint8_t kill(uint16_t pid);
uint8_t block(uint16_t pid);
uint8_t unblock(uint16_t pid);
void nice(uint16_t pid, uint8_t priority);
void yield(); // es la que libera al CPU voluntariamente
void wait_children();

#endif