#include "../include/scheduler.h"
#include <stdint.h>
#define AVAILABLE 1
#define MAX_INPUT 4
#include "../include/videoDriver.h"

uint8_t avail_inputs[MAX_INPUT];

int cmp(void * elem1, void * elem2) {
    return -1;
}

scheduler_struct * scheduler;

void init_scheduler(process_struct * pcb) {
    scheduler = my_malloc(sizeof(scheduler_struct));
    scheduler->schedule = newList(cmp);
    queue_first(scheduler->schedule,pcb);
    scheduler->current_running_pcb = pcb;
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
        } else if (current_pcb->status == KILLED){
            my_free(current_pcb->stack_base);
            my_free(current_pcb);
        }
    } while(current_pcb->status == BLOCKED || current_pcb->status == KILLED);
    scheduler->current_running_pcb = current_pcb;
    return current_pcb->stack_ptr;
}

uint16_t get_current_pid() {
    return scheduler->current_running_pcb->pid;
}