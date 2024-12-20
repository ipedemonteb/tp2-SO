#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "lib.h"
#include "memory_manager.h"
#include "process.h"
#include "scheduler.h"
#include "syscall.h"
#include <stdint.h>

typedef struct process_stack {
  void *r15;
  void *r14;
  void *r13;
  void *r12;
  void *r11;
  void *r10;
  void *r9;
  void *r8;
  void *rsi;
  void *rdi;
  void *rbp;
  void *rdx;
  void *rcx;
  void *rbx;
  void *rax;
  void *rip;
  void *cs;
  void *rflags;
  void *rsp;
  void *ss;
} process_stack;

void create_first_process(void (*fn)(uint8_t, char **), uint8_t argc,
                          char *argv[], const char *name);
int32_t create_process(void (*fn)(uint8_t, char **), uint8_t argc, char *argv[],
                       const char *name, uint8_t fg);
int8_t block(uint16_t pid);
int8_t unblock(uint16_t pid);
int8_t kill(uint16_t pid);
void nice(uint16_t pid, uint8_t priority);
void yield();
void wait_children();
uint8_t wait_pid(uint16_t pid, uint8_t block);
uint8_t ps(process_info *info);
void my_exit();

#endif