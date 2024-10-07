#include "../include/mmADT.h"
#include "../include/process_manager.h"
#include "../include/lib.h"

#define ALIGN 7

uint16_t current_pid = 0;
process_struct * processes[QUANT] = {0};

void launch_process(void (*fn)(uint8_t,uint8_t **), uint8_t argc , uint8_t * argv[]) {
    fn(argc, argv);
    exit();
}

void init_process(void * stack_base){
    process_struct * p_struct = my_malloc(sizeof(process_struct));
    processes[current_pid++] = p_struct;
    p_struct->argv = NULL;
    p_struct->name = "init";
    p_struct->stack_base = stack_base;
    init_scheduler(p_struct);
}

void load_proc_stack(process_struct * p_struct, void * stack) {
    p_struct->stack_base = stack;
    p_struct->pid = current_pid++;
    p_struct->priority = 1;  // TODO: ver
    p_struct->status = READY;
    p_struct->children_processes = 0;
    process_stack * p_stack = stack - sizeof(process_stack);
    p_stack->rsp = stack;
    p_stack->cs = (void *)0x8;
    p_stack->rflags = (void *)0x202;
    p_stack->ss = 0x0;
    p_stack->rip = launch_process;
    p_struct->stack_ptr = p_stack;
}

void load_proc_args(void (*fn)(uint8_t,uint8_t **), uint8_t argc, uint8_t * argv[], void * stack) {
    process_stack * p_stack = stack - sizeof(process_stack);
    p_stack->rdi = fn;
    p_stack->rsi = argc;
    p_stack->rdx = argv;
}

int32_t create_process(void (*fn)(uint8_t,uint8_t **), uint8_t argc, uint8_t * argv[]) { //@todo: ver si se devuelve un exit code por ejemplo (fn)
    if (current_pid >= QUANT)
    {
        return -1;
    }
    
    process_struct * p_struct = my_malloc(sizeof(process_struct));
    processes[current_pid] = p_struct;
    p_struct->argv = argv;
    void * stack = (uint64_t)(my_malloc(STACKSIZE) + STACKSIZE) & ~ALIGN;
    load_proc_stack(p_struct,stack);
    load_proc_args(fn, argc, argv, stack);
    schedule_process(p_struct);
    return current_pid - 1;
}

void exit(){
    kill(get_current_pid());
}

uint8_t kill(uint16_t pid) {
    processes[pid]->status = KILLED;
    if (get_current_pid() == pid) {
        int20();
    }
    // free_process_struct(); // TODO: crear esta funcion que libere el struct (que hacemos con el espacio en el vector??)
}

uint8_t block(uint16_t pid) {
    processes[pid]->status = BLOCKED;
    if (get_current_pid() == pid) {
        int20(); //ver que onda tema contextos ( se estarian guardando repetidos ahora )
    }
}

uint8_t unblock(uint16_t pid) {
    processes[pid]->status = READY;
}

void nice(uint16_t pid, uint8_t priority) {
    processes[pid]->priority = priority;
}

void yield() {
    int20(); //ver que onda tema prioridades
}

void wait_children() {
    // TODO: hacer
}