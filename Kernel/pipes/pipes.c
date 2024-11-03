#include "../include/pipes.h"
#include "../include/process.h"
#include "../include/scheduler.h"
#include "../include/process_manager.h"

#define IN 0
#define OUT 1

pipe_t pipes[MAX_BUFFERS] = {0};
uint64_t avail = 0;


int8_t pipe(int8_t id, uint8_t pipe_bd[2]){
    if (id < 2 || id >= MAX_BUFFERS) {
        return -1;
    }
    pipes[id].init = INIT;
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
    
    return id;
}

void close(uint8_t bd) {
    process_struct * pcb = get_process_info(get_current_pid());
    pcb->open_buffers[bd] = -1;
}

static uint8_t has_next_pipe(pipe_t * p) {
    return p->last > p->current;
}

static uint8_t next_from_buffer(pipe_t * p) {
	return p->buff[(p->current++) % BUFF_MAX];
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
    while (count < size && pipe->buff[pipes[id].current % BUFF_MAX] != EOF) {
        if (!has_next_pipe(pipe)) {
            pcb->status = BLOCKED;
            pcb->blocked_in = set_n_bit_64(pcb->blocked_in, id/2);
            yield();
        }
        arr[count++] = next_from_buffer(pipe);
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
        pipe->buff[pipe->last++] = buffer[count];
        count++;
    }
    pipe->buff[pipe->last] = EOF;
    avail = set_n_bit_64(avail,id/2);
    return count;
}

uint64_t get_available(){
    return avail;
}

pipe_t * get_keyboard_buffer(){
    return &pipes[0];
}

void keyboard_ready(){
    avail |= 1;
}