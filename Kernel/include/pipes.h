#ifndef PIPES_H
#define PIPES_H

#include <stdint.h>

#define BUFF_MAX 1024
#define EOF -1
#define INIT 1

typedef struct pipe_t {
    int8_t init;
    uint64_t last;
    uint64_t current;
    char buff[BUFF_MAX];
} pipe_t;

int8_t pipe(int8_t id, uint8_t pipe_bd[2]);

void close(uint8_t bd);

void copy(uint8_t dest_bd, uint8_t source_bd);

int64_t read(uint8_t bd, char * buffer, int64_t size);

int64_t write(uint8_t bd, char * buffer, int64_t size);

uint64_t get_available();

pipe_t * get_keyboard_buffer();

pipe_t * get_terminal_buffer();

void keyboard_ready();

void direct_write(pipe_t * p, char c);

uint8_t direct_read(pipe_t * p, char * c);


#endif