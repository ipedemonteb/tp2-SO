#include "../include/mmADT.h"
#include "../include/process_manager.h"

int current_pid = 0;

uint16_t createProcess(void * fn, uint8_t argc, uint8_t * argv[]) {
    process_struct * p_struct = my_malloc(sizeof(process_struct));
    void * stack = my_malloc(STACKSIZE) + STACKSIZE;
    processes[current_pid] = p_struct;
    p_struct->stack_base = stack;
    p_struct->pid = current_pid++;
    p_struct->argv = argv;
    p_struct->priority =  // TODO: ver
    p_struct->status = READY;
    p_struct->children_processes = 0;
    process_stack * p_stack = stack - sizeof(process_stack);
    p_stack->argc = argc;
    p_stack->argv = argv;
    p_stack->cs = (void *)0x8;
    p_stack->rflags = (void *)0x202;
    p_stack->ss = 0x0;
    p_stack->rip = fn;
    p_struct->stack_ptr = p_stack;
}

uint8_t kill(uint16_t pid) {
    processes[pid]->status = KILLED;
    // free_process_struct(); // TODO: crear esta funcion que libere el struct (que hacemos con el espacio en el vector??)
    // TODO: llamar al scheduler
}

uint8_t block(uint16_t pid) {
    processes[pid]->status = BLOCKED;
    // TODO: llamar al scheduler y que chequee que pasa

}

uint8_t unblock(uint16_t pid) {
    processes[pid]->status = READY;
}

void nice(uint16_t pid, uint8_t priority) {
    processes[pid]->priority = priority;
}

void yield() {
    // TODO: hacer (llamar al scheduler)
}

void wait_children() {
    // TODO: hacer
}