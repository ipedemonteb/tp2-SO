#ifndef TERMINAL_DRIVER_H
#define TERMINAL_DRIVER_H

#include <stdint.h>

#define CTRL_C -56
#define CTRL_D -1
#define DELETE 127
#define LEFT_ARROW -5
#define RIGHT_ARROW -4
#define UP_ARROW -3
#define DOWN_ARROW -2

typedef struct t_char{
    char character;
    uint32_t fg_colour;
    uint32_t bg_colour;
}t_char;

void t_off_cursor();

void t_set_cursor();

void t_insert_char(char c);

void t_remove_char();

void t_draw_line(char * line, uint8_t new);

void t_kill_fg();

void add_to_fg(uint16_t pid);

void start_terminal();

void terminal();

#endif