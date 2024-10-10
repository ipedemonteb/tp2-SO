#include "../include/scheduler.h"
#include <stdint.h>
#define AVAILABLE 1
#define MAX_INPUT 4
#include "../include/videoDriver.h"

#include "test_util.h" // TODO: borrar


uint8_t avail_inputs[MAX_INPUT];

int cmp(void * elem1, void * elem2) {
    return -1;
}

uint8_t debug = 0;
scheduler_struct * scheduler;

void init_scheduler(void * stack_base) {
    scheduler = my_malloc(sizeof(scheduler_struct));
    scheduler->schedule = newList(cmp);

    scheduler->current_running_pcb = my_malloc(sizeof(process_struct));
    scheduler->current_running_pcb->pid = 0;
    scheduler->current_running_pcb->priority = 1;
    scheduler->current_running_pcb->count = 1;
    scheduler->current_running_pcb->name = "kernel";
    scheduler->current_running_pcb->stack_base = stack_base;
}

void schedule_process(process_struct * pcb){
    add(scheduler->schedule,pcb);
}

void * schedule(void * rsp) {   // round robin with priorities scheduling
    drawchar(get_current_pid() + '0' , debug++ , 0 , WHITE , BLACK );
    scheduler->current_running_pcb->stack_ptr = rsp;
    while (scheduler->current_running_pcb->count <= scheduler->current_running_pcb->priority) {
        add(scheduler->schedule, scheduler->current_running_pcb);
        scheduler->current_running_pcb->count++;
    }
    scheduler->current_running_pcb->count--;
    process_struct * current_pcb;
    do {
        current_pcb = poll(scheduler->schedule);
        if(current_pcb->status == BLOCKED) {
            if(avail_inputs[current_pcb->blocked_in] == AVAILABLE) {
                current_pcb->status = READY;
            } else {
                add(scheduler->schedule, current_pcb);
            }
        } 
    } while(current_pcb->status == BLOCKED || current_pcb->status == KILLED);
    scheduler->current_running_pcb = current_pcb;
    return current_pcb->stack_ptr;
}

uint16_t get_current_pid() {
    return scheduler->current_running_pcb->pid;
}