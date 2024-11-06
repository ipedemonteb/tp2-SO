#ifndef TERMINAL_DRIVER_H
#define TERMINAL_DRIVER_H

#include <stdint.h>

#define CTRL_C 200

typedef struct t_char{
    char character;
    uint32_t fg_colour;
    uint32_t bg_colour;
}t_char;

void t_off_cursor();

void t_set_cursor();

void t_insert_char(char c);

void t_draw_line(char * line, uint8_t new);

void start_terminal();

void terminal();

#endif