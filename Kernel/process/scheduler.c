// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/scheduler.h"
#include "../include/pipes.h"
#include "../include/video_driver.h"
#include <stdint.h>

#define AVAILABLE 1
#define MAX_INPUT 64

int cmp(process_struct *elem1, process_struct *elem2) {
  return elem1->pid - elem2->pid;
}

uint8_t debug = 0;
scheduler_struct *scheduler;

void init_scheduler(void *stack_base) {
  scheduler = my_malloc(sizeof(scheduler_struct));
  for (uint8_t i = 0; i < MAX_PRIO; i++) {
    scheduler->schedule[i] = new_list((void *)cmp);
  }

  scheduler->current_running_pcb = my_malloc(sizeof(process_struct));
  scheduler->current_running_pcb->status = KILLED;
  scheduler->current_running_pcb->stack_base = stack_base;
  for (uint8_t i = 0; i < MAX_PRIO; i++) {
      scheduler->count[i] = 0;
  }
}

void deschedule_process(process_struct *pcb) {
  remove(scheduler->schedule[pcb->priority], pcb);
}

void schedule_process(process_struct *pcb) {
  queue(scheduler->schedule[pcb->priority], pcb);
}

void *schedule(void *rsp) {
  scheduler->current_running_pcb->stack_ptr = rsp;
  if (scheduler->count[0] == 1) {
    for (uint8_t i = 0; i < MAX_PRIO; i++) {
      scheduler->count[i] = 0;
    }
  }
  
  if (scheduler->current_running_pcb->status == READY ) {
    queue(scheduler->schedule[scheduler->current_running_pcb->priority],
          scheduler->current_running_pcb);
  }

  for (int8_t i = MAX_PRIO - 1; i >= 0; i--) {
    if (!is_empty(scheduler->schedule[i]) && scheduler->count[i] <= (i * 5 + 1)) {
      scheduler->current_running_pcb = poll(scheduler->schedule[i]);
      (scheduler->count[i])++;
      break;
    }
  }
  return scheduler->current_running_pcb->stack_ptr;
}

uint16_t get_current_pid() { return scheduler->current_running_pcb->pid; }