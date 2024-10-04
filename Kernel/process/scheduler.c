#include "../include/scheduler.h"
#include <stdint.h>
#define AVAILABLE 1
#define MAX_INPUT 4

uint8_t avail_inputs[MAX_INPUT];

int cmp(void * elem1, void * elem2) {
    return -1;
}

scheduler_struct * scheduler;

void init_scheduler() {
    scheduler = my_malloc(sizeof(scheduler_struct));
    scheduler->schedule = newList(cmp);
}

void schedule_process(process_struct * pcb){
    queue(scheduler->schedule,pcb);
}

void * schedule(void * rsp) {   // round robin with priorities scheduling
    scheduler->current_running_pcb->stack_ptr = rsp;
    queue(scheduler->schedule, scheduler->current_running_pcb);
    process_struct * current_pcb;
    do {
        current_pcb = poll(scheduler->schedule);
        if(current_pcb->status == BLOCKED) {
            if(avail_inputs[current_pcb->blocked_in] == AVAILABLE) {
                current_pcb->status = READY;
            } else {
                queue(scheduler->schedule, current_pcb);
            }
        }
        //TODO: ver que hacemos con los matados
    } while(current_pcb->status == BLOCKED || current_pcb->status == KILLED);
    return current_pcb->stack_ptr;
}

uint16_t get_current_pid() {
    return scheduler->current_running_pcb->pid;
}