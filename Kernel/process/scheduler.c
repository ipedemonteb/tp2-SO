#include "../include/scheduler.h"
#include <stdint.h>

process_struct * processes[QUANT] = {0};


int cmp(void * elem1, void * elem2) {
    return 1;
}

scheduler_struct * scheduler;

void init_scheduler() {
    scheduler = my_malloc(sizeof(scheduler_struct));
    scheduler->schedule = newList(cmp);
}

void schedule(void * rsp) {   // round robin with priorities scheduling
    add(scheduler->schedule, )
    uint16_t next_pid = next(scheduler->schedule);

}

uint16_t get_current_pid() {
    return scheduler->current_running_pid;
}