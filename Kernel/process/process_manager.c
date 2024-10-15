#include "../include/memory_manager.h"
#include "../include/process_manager.h"
#include "../include/lib.h"
#include <stdint.h>

#include "../include/videoDriver.h"
#include "../include/test_util.h"
#include "../include/interrupts.h"

#define ALIGN 7 

uint8_t proc_count = 0;
process_struct processes[QUANT] = {0};

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
    p_struct->parent_pcb = &processes[get_current_pid() % QUANT];
    p_struct->parent_pcb->children_processes[(p_struct->pid)/64] = set_n_bit_64(p_struct->parent_pcb->children_processes[(p_struct->pid)/64], (p_struct->pid) % 64);
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

int32_t create_process(void (*fn)(uint8_t,uint8_t **), uint8_t argc, uint8_t * argv[], int8_t * name) { //@todo: ver si se devuelve un exit code por ejemplo (fn)
    if(proc_count >= QUANT){
        return -1;
    }
    uint16_t pid = find_off_bit_128(occupied[0], occupied[1]);
    occupied[pid/64] = set_n_bit_64(occupied[pid/64], pid % 64);
    proc_count++;
    process_struct * p_struct = &processes[pid];
    p_struct->pid = pid;
    p_struct->argv = argv;
    p_struct->name = my_malloc(my_strlen(name) + 1);
    memcpy(p_struct->name, name, my_strlen(name));
    void * stack = (uint64_t)(my_malloc(STACKSIZE) + STACKSIZE) & ~ALIGN;
    load_proc_stack(p_struct, stack);
    load_proc_args(fn, argc, argv, stack);
    schedule_process(p_struct);
    return p_struct->pid;
}

uint8_t kill(uint16_t pid) {
    processes[pid % QUANT].status = KILLED;
    if (get_current_pid() == pid) {
        yield();
    }
}

uint8_t block(uint16_t pid) {
    processes[pid % QUANT].status = BLOCKED;
    if (get_current_pid() == pid) {
        yield();
    }
}

uint8_t unblock(uint16_t pid) {
    processes[pid % QUANT].status = READY;
}

void nice(uint16_t pid, uint8_t priority) {
    processes[pid % QUANT].priority = priority;
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
            memcpy(info[process_count].name, p->name, my_strlen(p->name));
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

uint16_t help = 0;

void wait_children() {
    uint16_t pid = get_current_pid();
    uint8_t child, idx;
    process_struct * pcb = &processes[pid % QUANT];
    while(pcb->children_processes[0] && pcb->children_processes[1]){
        uint16_t p = 0;
        int8_t aux[30];
       /*  numToStr(pcb->killed_children[0], aux);
        drawString(aux, 0, 23 + help, WHITE, BLACK);
        numToStr(pcb->killed_children[1], aux);
        drawString(aux, 0, 24 + help, WHITE, BLACK); */
        while (pcb->killed_children[0] || pcb->killed_children[1]) {
            child = find_off_bit_128(~pcb->killed_children[0], ~pcb->killed_children[1]);
            idx = child/64;
            pcb->children_processes[idx] = off_n_bit_64(pcb->children_processes[idx],child%64);
            pcb->killed_children[idx] = off_n_bit_64(pcb->killed_children[idx],child%64);
            occupied[idx] = off_n_bit_64(occupied[idx],child%64);
            //drawchar('x', p++, 20, WHITE, BLACK);
            /* numToStr(pcb->killed_children[idx], aux);
            drawString(aux, 0, 25 + help, WHITE, BLACK);
            numToStr(child, aux);
            drawString(aux, 0, 26 + help, WHITE, BLACK);
            help += 4; */
        }
        if (pcb->children_processes[0] && pcb->children_processes[1]) {
            pcb->status = BLOCKED; 
            yield();
        }    
    }
}