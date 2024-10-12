#include "../include/mmADT.h"
#include "../include/process_manager.h"
#include "../include/lib.h"

#define ALIGN 7 

process_struct processes[QUANT] = {0};

uint64_t occupied[2];

void launch_process(void (*fn)(uint8_t,uint8_t **), uint8_t argc , uint8_t * argv[]) {
    fn(argc, argv);
    exit();
}

void load_proc_stack(process_struct * p_struct, void * stack) {
    p_struct->stack_base = stack;
    occupied[p_struct->pid/64] = set_n_bit_64(occupied[p_struct->pid/64],p_struct->pid % 64);
    p_struct->priority = 1; 
    p_struct->status = READY;
    p_struct->parent_pcb = &processes[get_current_pid() % QUANT];
    p_struct->children_processes[(p_struct->pid)/64] = set_n_bit_64(p_struct->children_processes[(p_struct->pid)/64], (p_struct->pid) % 64);
    p_struct->children_processes[0] = 0;
    p_struct->children_processes[1] = 0;
    p_struct->killed_children[0] = 0;
    p_struct->killed_children[1] = 0;
    p_struct->count = 1;
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
    /* if (current_pid >= QUANT) {
        return -1;
    } */
    
    //process_struct * p_struct = my_malloc(sizeof(process_struct));
    //processes[current_pid] = p_struct;
    uint16_t pid = find_off_bit_128(occupied[0], occupied[1]);
    process_struct * p_struct = &processes[pid];
    p_struct->pid = pid;
    p_struct->argv = argv;
    void * stack = (uint64_t)(my_malloc(STACKSIZE) + STACKSIZE) & ~ALIGN;
    load_proc_stack(p_struct,stack);
    load_proc_args(fn, argc, argv, stack);
    schedule_process(p_struct);
    return p_struct->pid;
}

void exit(){
    kill(get_current_pid());
}

uint8_t kill(uint16_t pid) {
    /* if (pid > current_pid) {
        return -1;
    } */
    
    processes[pid % QUANT].status = KILLED;
    if (get_current_pid() == pid) {
        int20();
    }
}

uint8_t block(uint16_t pid) {
    processes[pid % QUANT].status = BLOCKED;
    if (get_current_pid() == pid) {
        int20(); //ver que onda tema contextos ( se estarian guardando repetidos ahora )
    }
}

uint8_t unblock(uint16_t pid) {
    processes[pid % QUANT].status = READY;
}

void nice(uint16_t pid, uint8_t priority) {
    processes[pid % QUANT].priority = priority;
}

void yield() {
    int20(); //ver que onda tema prioridades
}

void ps() {
    
}

void wait_children() {
    uint16_t pid = get_current_pid();
    uint8_t child, idx;
    process_struct * pcb = &processes[pid % QUANT];
    while(pcb->children_processes[0] && pcb->children_processes[1]){
        pcb->status = BLOCKED; //@todo: ver esto de bloquear de antemano
        yield();
        while (pcb->killed_children[0] || pcb->killed_children[1]) {
            child = find_off_bit_128(~pcb->killed_children[0], ~pcb->killed_children[1]);
            idx = child/64;
            pcb->children_processes[idx] = off_n_bit_64(pcb->children_processes[idx],child);
            occupied[idx] = off_n_bit_64(occupied[idx],child);
        }
    }
}