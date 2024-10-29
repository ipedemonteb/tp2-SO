#ifndef SHELL_GRAPHICS_H
#define SHELL_GRAPHICS_H

#include <stdint.h>

typedef struct s_char{
    char character;
    uint32_t fg_colour;
    uint32_t bg_colour;
}s_char;

void s_start_graphics(s_char * new_prompt, uint8_t len);

void s_insert_char(char c);

void s_draw_line(char * line, uint8_t with_prompt, uint8_t newL);

void s_move_cursor(int8_t amount);

void s_set_cursor();

void s_off_cursor();

int8_t s_move_screen_up(uint8_t amount);

void s_remove_char();

int8_t s_increase_font_size();

int8_t s_decrease_font_size();

void s_clear();

#endif