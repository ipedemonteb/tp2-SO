// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/pipes.h"
#include "../include/process.h"
#include "../include/scheduler.h"
#include "../include/process_manager.h"
#include "../include/terminal_driver.h"

#define TERMINAL 1
#define KEYBOARD 0

pipe_t pipes[MAX_BUFFERS] = {0};
uint64_t avail = 0;


int8_t pipe(int8_t id, uint8_t pipe_bd[2]){
    if (id == 0 || id == 1 || id >= MAX_BUFFERS) {
        return -1;
    }

    if (id == ANON) {
        id = 0;
        while (pipes[id].init) {
            id++;
        }
        
    }
    
    pipes[id].init = P_INIT;
    process_struct * pcb = get_process_info(get_current_pid());
    uint8_t count = 0;
    for (uint8_t i = 0; count < 2 && i < MAX_BUFFERS; i++) {
        if (pcb->open_buffers[i] == -1) {
            pipe_bd[count] = i;
            pcb->open_buffers[i] = id * 2 + count;
            count++;
        }
    }
    if (count < 2) {
        if (count == 1) {
            pcb->open_buffers[pipe_bd[count - 1]] = -1;
        }
        return -1;
    } 
    pipes[id].opened_read++;
    pipes[id].opened_write++;

    return id;
}

void internal_close(process_struct * pcb, uint8_t bd) {
    if(pcb->open_buffers[bd] == -1) return;
    uint8_t id = pcb->open_buffers[bd]/2;
    if (pcb->open_buffers[bd] % 2 == OUT) {
        pipes[id].opened_write--;
        if (!pipes[id].opened_write) {
            pipes[id].buff[(pipes[id].last++) % BUFF_MAX] = EOF;
        }
    } else {
        pipes[id].opened_read--;
    }
    if(!pipes[id].opened_read && !pipes[id].opened_write) {
        pipes[id].current = pipes[id].last;
        pipes[id].init = P_NOT_INIT;
    }
    pcb->open_buffers[bd] = -1;
}

void close(uint8_t bd) {
    internal_close(get_process_info(get_current_pid()), bd);
}

void internal_copy(process_struct * pcb, uint8_t dest_bd, uint8_t source_bd) {
    internal_close(pcb, dest_bd);
    pcb->open_buffers[dest_bd] = pcb->open_buffers[source_bd];
    if (pcb->open_buffers[dest_bd] == -1) return;
    
    uint8_t id = pcb->open_buffers[dest_bd]/2;
    if (pcb->open_buffers[dest_bd] % 2 == OUT) {
        pipes[id].opened_write++;
    } else {
        pipes[id].opened_read++;
    }
}

void copy(uint8_t dest_bd, uint8_t source_bd) {
    internal_copy(get_process_info(get_current_pid()), dest_bd, source_bd);
}

void swap(uint8_t bd1, uint8_t bd2) {
    process_struct * pcb = get_process_info(get_current_pid());
    int8_t aux = pcb->open_buffers[bd1];
    pcb->open_buffers[bd1] = pcb->open_buffers[bd2];
    pcb->open_buffers[bd2] = aux;
}

static uint8_t has_next_pipe(pipe_t * p) {
    return p->last > p->current;
}

static uint8_t next_from_buffer(pipe_t * p) {
	return p->buff[(p->current++) % BUFF_MAX];
}

void pipe_ready(pipe_t * pipe, uint8_t r_w) {
    uint8_t pid;
    uint64_t * arr;
    if(r_w) {
        arr = pipe->blocked_read;
    } else {
        arr = pipe->blocked_write;
    }
    while(arr[0] || arr[1]) {
        pid = find_off_bit_128(~arr[0], ~arr[1]);
        unblock(pid);
        arr[pid / 64] = off_n_bit_64(arr[pid / 64], pid % 64);
    }
}

int64_t read(uint8_t bd, char * arr, int64_t size) {
    process_struct * pcb = get_process_info(get_current_pid());
    int8_t id = pcb->open_buffers[bd];
    if (id % 2 == OUT) {
        return -1;
    }

    pipe_t * pipe = &pipes[id/2];  
    if (!pipe->init) {
        return -1;
    }
    
    int64_t count = 0;
    while (count < size) {
        if (!has_next_pipe(pipe)) {
            pipe->blocked_read[pcb->pid / 64] = set_n_bit_64(pipe->blocked_read[pcb->pid / 64], pcb->pid % 64);
            block(pcb->pid);
        } else {
            if (pipe->buff[pipe->current % BUFF_MAX] == EOF) { //@todo: ver si solo hacerlo con el teclado esto
                pipe->current++;
                break;
            }
            arr[count++] = next_from_buffer(pipe);
            pipe_ready(pipe, 0);
        }
    }
    
    pcb->blocked_in = off_n_bit_64(pcb->blocked_in, id/2);
    if (!has_next_pipe(pipe)) {
        avail = off_n_bit_64(avail, id/2);
    }
    
    return count;
}


int64_t write(uint8_t bd, char * buffer, int64_t size) {
    process_struct * pcb = get_process_info(get_current_pid());
    int8_t id = pcb->open_buffers[bd];
    if (id % 2 == IN) {
        return -1;
    }

    pipe_t * pipe = &pipes[id/2];
    if (!pipe->init) {
        return -1;
    }

    int64_t count = 0;
    while (count < size) {
        if (pipe->last - pipe->current >= BUFF_MAX) {
            pipe->blocked_write[pcb->pid / 64] = set_n_bit_64(pipe->blocked_write[pcb->pid / 64], pcb->pid % 64);
            block(pcb->pid);
        }
        
        pipe->buff[(pipe->last++) % BUFF_MAX] = buffer[count];
        count++;
        if (id/2 == TERMINAL) terminal();
        else pipe_ready(pipe, 1);
    }
    
    return count;
}

uint64_t get_available(){
    return avail;
}

pipe_t * get_pipe_by_id(uint8_t id) {
    return &pipes[id];
}

pipe_t * get_keyboard_buffer(){
    return &pipes[KEYBOARD];
}

pipe_t * get_terminal_buffer() {
    return &pipes[TERMINAL];
}

void keyboard_ready(){
    pipe_ready(&pipes[0], 1);
}

void direct_write(pipe_t * p, char c) {
    p->buff[p->last % BUFF_MAX] = c;
    p->last++;
}

uint8_t direct_read(pipe_t * p, char * c) {
    if (p->current >= p->last ) {
        return 0;
    }
    *c = p->buff[p->current % BUFF_MAX];
    p->current++;
    return 1;
}