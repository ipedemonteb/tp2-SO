#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>
#include "memory_manager.h"
#include "listADT.h"
#include "process.h"
#include "syscall.h"
#include "lib.h"
#define MAX_PRIO 5

typedef struct scheduler_struct {
    listADT schedule[MAX_PRIO];
    process_struct * current_running_pcb;
} scheduler_struct;

void init_scheduler(void * stack_base);
void schedule_process(process_struct * pcb);
void deschedule_process(process_struct * pcb);
void * schedule(void * rsp);
uint16_t get_current_pid();
void get_next_process();

#endif
