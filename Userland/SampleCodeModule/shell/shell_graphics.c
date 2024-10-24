#include "../include/shell_graphics.h"
#include "../include/syscall.h"
#include "../include/string_arrayADT.h"

#define WHITE 0x00FFFFFF
#define BLACK 0
#define PROMPT_MAX 32

static uint16_t currentY;
static uint32_t currentX;
static uint16_t width;
static uint16_t height;
static char prompt[PROMPT_MAX];
static uint8_t prompt_len;
static uint8_t char_height;
static uint8_t char_width;

string_arrayADT commands;
string_arrayADT command_outputs;

void s_start_graphics(char * new_prompt) {
    currentX = 0;
    currentY = 0;
    width = getScreenWidth()/char_width;
    height = getScreenHeight()/char_height;
    uint8_t i;
    for (i = 0; i < PROMPT_MAX - 1 && new_prompt[i]; i++) {
        prompt[i] = new_prompt[i];
    }
    prompt[i] = 0;
    prompt_len = i;
}

void s_new_command(){
    currentX = 0;
    while(prompt[currentX]) {
        printChar(prompt[currentX], currentX % width, currentY, WHITE, BLACK);
        currentX++;
    }
}

void s_print_char(char c) {
    printChar(c, currentX, currentY, WHITE, BLACK);
}

void s_draw_line(char * line, uint8_t with_prompt) {
    currentX = 0;
    if (width_prompt) {
        s_new_command();
    }
    uint8_t i = 0;
    while(line[i]) {
        s_print_char(line[i]);
        if (currentX >= width - 1) {
            currentY++
        }
        currentX++;
    }
}

void s_clear_line() {
    printRectangle(0, currentY * char_height, width, char_height, BLACK);
}

void s_move_screen_up(uint8_t ammount) {
    currentY = 0;
}

void s_move_cursor(int8_t ammount, char current, char next) {
    if (currentX + ammount < 0) {
        return;
    }
    printChar(current, currentX % width, currentY, WHITE, BLACK);
    if (currentX + ammount >= width ) {
        if (currentY == height - 1) {
            s_move_screen_up(1);
        }else {
            currentY++;
        }  
    }
    currentX += amount;
    printChar(next, currentX % width, currentY, BLACK, WHITE);
}

void s_clear(){
    printRectangle(0, 0, width, height, BLACK);
}