#include "../include/scheduler.h"
#include <stdint.h>
#include "../include/videoDriver.h"
#define AVAILABLE 1
#define MAX_INPUT 4


uint8_t avail_inputs[MAX_INPUT] = {0};

int cmp(void * elem1, void * elem2) {
    return -1;
}

uint8_t debug = 0;
scheduler_struct * scheduler;

void init_scheduler(void * stack_base) {
    scheduler = my_malloc(sizeof(scheduler_struct));
    scheduler->schedule = newList(cmp);

    scheduler->current_running_pcb = my_malloc(sizeof(process_struct));
    scheduler->current_running_pcb->pid = 130;
    scheduler->current_running_pcb->priority = 1;
    scheduler->current_running_pcb->count = 2;
    scheduler->current_running_pcb->stack_base = stack_base;
}

void schedule_process(process_struct * pcb) {
    add(scheduler->schedule,pcb);
}

//In development
void * schedule(void * rsp) {
    //print
    drawchar(get_current_pid() + '0', debug++, 1, WHITE, BLACK);
    scheduler->current_running_pcb->stack_ptr = rsp;
    while (scheduler->current_running_pcb->count <= scheduler->current_running_pcb->priority) {
        add(scheduler->schedule, scheduler->current_running_pcb);
        scheduler->current_running_pcb->count++;
    }
    scheduler->current_running_pcb->count--;
    process_struct * current_pcb;
    do {
        current_pcb = poll(scheduler->schedule);
        /*    if(avail_inputs[current_pcb->blocked_in] == AVAILABLE) {
                current_pcb->status = READY;
            } else {
            }
        } else  */
        if(current_pcb->status == BLOCKED) {
            add(scheduler->schedule, current_pcb);
        }
        else if(current_pcb->status == KILLED) {
            current_pcb->count--;
            if (current_pcb->count == 0) {
                uint8_t pid = current_pcb->pid, idx = pid/64;
                /* int8_t aux[30];
                numToStr(pid, aux);
                drawString(aux, 0, t++, WHITE, BLACK);  */
                current_pcb->parent_pcb->killed_children[idx] = set_n_bit_64(current_pcb->parent_pcb->killed_children[idx],pid % 64);
                current_pcb->parent_pcb->status = READY;
                current_pcb->parent_pcb->children_processes[0] |= current_pcb->children_processes[0];
                current_pcb->parent_pcb->children_processes[1] |= current_pcb->children_processes[1];
            }
        }
    } while(current_pcb->status == BLOCKED || current_pcb->status == KILLED);
    scheduler->current_running_pcb = current_pcb;
    return current_pcb->stack_ptr;
}

uint16_t get_current_pid() {
    return scheduler->current_running_pcb->pid;
}

void get_next_process() {
    while(scheduler->current_running_pcb->pid == ((process_struct*)peek(scheduler->schedule))->pid) {
        process_struct * aux = poll(scheduler->schedule);
        add(scheduler->schedule, aux);
    }
}