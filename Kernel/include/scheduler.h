#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>
#include "memory_manager.h"
#include "listADT.h"
#include "process.h"

typedef struct scheduler_struct {
    listADT schedule;
    process_struct * current_running_pcb;
} scheduler_struct;

void init_scheduler();
void schedule_process();
void * schedule();
uint16_t get_current_pid();

#endif
