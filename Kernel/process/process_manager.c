#include "../include/memory_manager.h"
#include "../include/process_manager.h"
#include "../include/lib.h"
#include <stdint.h>

#define ALIGN 7 

uint8_t proc_count = 0;
process_struct processes[QUANT] = {0};

void * stacks = (void *)0x1000000;

uint64_t occupied[2];

void my_exit() {
    kill(get_current_pid());
}

void launch_process(void (*fn)(uint8_t,uint8_t **), uint8_t argc , uint8_t * argv[]) {
    fn(argc, argv);
    my_exit();
}

void load_proc_stack(process_struct * p_struct, void * stack) {
    p_struct->stack_base = stack;
    p_struct->priority = 1; 
    p_struct->status = READY;
    p_struct->parent_pcb = &processes[get_current_pid()];
    
    p_struct->parent_pcb->children_processes[(p_struct->pid)/64] = set_n_bit_64(p_struct->parent_pcb->children_processes[(p_struct->pid)/64], (p_struct->pid) % 64);
    p_struct->children_processes[0] = 0; //@todo: ver si son necesarias estas inicializaciones
    p_struct->children_processes[1] = 0;
    p_struct->killed_children[0] = 0;
    p_struct->killed_children[1] = 0;
    p_struct->count = 1;

    for (uint8_t i = 0; i < MAX_BUFFERS; i++) {
        p_struct->open_buffers[i] = p_struct->parent_pcb->open_buffers[i];
    }
    
    process_stack * p_stack = stack - sizeof(process_stack);
    p_stack->rsp = stack;
    p_stack->rbp = stack;
    p_stack->cs = (void *)0x8;
    p_stack->rflags = (void *)0x202;
    p_stack->ss = 0x0;
    p_stack->rip = launch_process;
    p_struct->stack_ptr = p_stack;
}

void load_proc_args(void (*fn)(uint8_t,uint8_t **), uint8_t argc, uint8_t * argv[], void * stack) {
    process_stack * p_stack = stack - sizeof(process_stack);
    p_stack->rdi = fn;
    p_stack->rsi = (void *)(uintptr_t)argc;
    p_stack->rdx = argv;
}

int32_t create_process(void (*fn)(uint8_t,uint8_t **), uint8_t argc, uint8_t * argv[], const char * name) { //@todo: ver si se devuelve un exit code por ejemplo (fn)
    if(proc_count >= QUANT){
        return -1;
    }
    uint16_t pid = find_off_bit_128(occupied[0], occupied[1]);
    occupied[pid/64] = set_n_bit_64(occupied[pid/64], pid % 64);
    process_struct * p_struct = &processes[pid];
    p_struct->pid = pid;
    p_struct->argv = argv;
    p_struct->name = my_malloc(my_strlen(name) + 1);
    memcpy(p_struct->name, name, my_strlen(name));
    void * stack = stacks + STACKSIZE * (pid+1);
    load_proc_stack(p_struct, stack);
    load_proc_args(fn, argc, argv, stack);
    schedule_process(p_struct);
    return p_struct->pid;
}

void create_first_process(void (*fn)(uint8_t,uint8_t **), uint8_t argc, uint8_t * argv[], const char * name) {
    process_struct * p_struct = &processes[0];
    p_struct->pid = 0;
    occupied[0] |= 1; 
    p_struct->argv = argv;
    p_struct->name = my_malloc(my_strlen(name) + 1);
    memcpy(p_struct->name, name, my_strlen(name));
    void * stack = stacks + STACKSIZE;

    p_struct->stack_base = stack;
    p_struct->priority = 1; 
    p_struct->status = READY;
    p_struct->parent_pcb = &processes[0];//@todo: ver si es preferible que el primero apunte a si mismo
    p_struct->children_processes[0] = 0;
    p_struct->children_processes[1] = 0;
    p_struct->killed_children[0] = 0;
    p_struct->killed_children[1] = 0;
    p_struct->count = 1;

    for (uint8_t i = 0; i < MAX_BUFFERS; i++) {
        p_struct->open_buffers[i] = i < 4 ? i : -1;
    }

    process_stack * p_stack = stack - sizeof(process_stack);
    p_stack->rsp = stack;
    p_stack->rbp = stack;
    p_stack->cs = (void *)0x8;
    p_stack->rflags = (void *)0x202;
    p_stack->ss = 0x0;
    p_stack->rip = launch_process;
    p_struct->stack_ptr = p_stack;
    p_stack->rdi = fn;
    p_stack->rsi = (void *)(uintptr_t)argc;
    p_stack->rdx = argv;
    schedule_process(p_struct);
}

uint8_t kill(uint16_t pid) {
    processes[pid].status = KILLED;
    if (get_current_pid() == pid) {
        yield();
    }
    return 0;
}

uint8_t block(uint16_t pid) {
    processes[pid].status = BLOCKED;
    if (get_current_pid() == pid) {
        yield();
    }
    return 0;
}

uint8_t unblock(uint16_t pid) {
  if (processes[pid].status != KILLED) {
    processes[pid].status = READY;
  }
  return 0;
}

void nice(uint16_t pid, uint8_t priority) {
    processes[pid].priority = priority;
}

void yield() {
    get_next_process();
    int20();
}

uint8_t ps(process_info * info) {
    uint8_t process_count = 0;
    for(int i = 0; i < QUANT; i++) {
        if(occupied[i / 64] & (1UL << (i % 64))) {
            process_struct * p = &processes[i];
            info[process_count].name = my_malloc(my_strlen(p->name) + 1);
            memcpy(info[process_count].name, p->name, my_strlen(p->name) + 1);
            info[process_count].pid = p->pid;
            info[process_count].priority = p->priority;
            info[process_count].stack_base = p->stack_base;
            info[process_count].stack_ptr = p->stack_ptr;
            info[process_count].foreground = 0;
            info[process_count].status = p->status;
            process_count++;
        }
    }
    return process_count;
}

void wait_children() {
    uint16_t pid = get_current_pid();
    uint8_t child, idx;
    process_struct * pcb = &processes[pid];
    while(pcb->children_processes[0] || pcb->children_processes[1]){
        while (pcb->killed_children[0] || pcb->killed_children[1]) {
            child = find_off_bit_128(~pcb->killed_children[0], ~pcb->killed_children[1]);
            idx = child/64;
            pcb->children_processes[idx] = off_n_bit_64(pcb->children_processes[idx],child%64);
            pcb->killed_children[idx] = off_n_bit_64(pcb->killed_children[idx],child%64);
            occupied[idx] = off_n_bit_64(occupied[idx],child%64);
            my_free(processes[child].name);
        }
        if (pcb->children_processes[0] || pcb->children_processes[1]) {
            pcb->status = BLOCKED; 
            yield();
        }    
    }
}

void wait_pid(uint16_t pid) {
    process_struct * parent_pcb = &processes[get_current_pid()];
    if (!(parent_pcb->children_processes[pid / 64] & (1UL << (pid % 64)))) {
        return;
    }

    while (!(parent_pcb->killed_children[pid / 64] & (1UL << (pid % 64)))) {
        parent_pcb->status = BLOCKED;
        yield();
    }

    parent_pcb->children_processes[pid / 64] = off_n_bit_64(parent_pcb->children_processes[pid / 64], pid % 64);
    parent_pcb->killed_children[pid / 64] = off_n_bit_64(parent_pcb->killed_children[pid / 64], pid % 64);
    occupied[pid / 64] = off_n_bit_64(occupied[pid / 64], pid % 64);
    my_free(processes[pid].name);
}

process_struct * get_process_info(uint8_t pid) {
    return &processes[pid];
}