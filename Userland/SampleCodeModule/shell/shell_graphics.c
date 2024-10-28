#include "../include/shell_graphics.h"
#include "../include/syscall.h"
#include "../include/string_arrayADT.h"

#define WHITE 0x00FFFFFF
#define BLACK 0
#define PROMPT_MAX 16
#define MAX_SCREEN 6500

static uint16_t lastX;

static uint32_t currentX;

s_char screen[MAX_SCREEN];

static uint16_t width;
static uint16_t height;
static uint8_t char_height;
static uint8_t char_width;

static s_char prompt[PROMPT_MAX];
static uint8_t prompt_len;

static s_char blanck = {0, BLACK, BLACK};

void re_draw_line(uint32_t from, uint32_t to){
    for(uint16_t i = from; i < to ; i++) {
        printChar(screen[i].character, i % width , i / width, screen[i].fg_colour, screen[i].bg_colour);
    }
}

void re_draw_screen(){
    re_draw_line(0,MAX_SCREEN);
}

void s_start_graphics(s_char * new_prompt, uint8_t len) {
    currentX = 0;
    lastX = 0;
    char_height = 16;
    char_width = 8;
    width = getScreenWidth() / char_width;
    height = getScreenHeight() / char_height;
    for(uint16_t i = 0; i < MAX_SCREEN; i++){
        screen[i] = blanck;
    }
    re_draw_screen();
    uint8_t i;
    for (i = 0; i < PROMPT_MAX - 1 && i < len; i++) {
        prompt[i] = new_prompt[i];
    }
    prompt_len = i;
}

void start_line(uint8_t new){
    if (new) {
        if (currentX / width >= height - 1) {
            s_move_screen_up(1);
        }
        currentX += width;
    }
    currentX = (currentX / width) * width;
}

void set_command_prompt(){
    uint16_t i = 0;
    while(i < prompt_len) {
        screen[currentX] = prompt[i++];
        currentX++;
    }
}

void set_cursor(){
    s_move_cursor(0);
}

void s_draw_line(char * line, uint8_t with_prompt, uint8_t new) {
    start_line(new);
    uint32_t start_position = currentX;
    if (with_prompt) {
        set_command_prompt();
    }
    uint16_t i = 0;
    while(line[i]) {
        screen[currentX].character = line[i++];
        screen[currentX].bg_colour = BLACK;
        screen[currentX].fg_colour = WHITE; 
        currentX++;
    }
    i = currentX;
    while (screen[currentX].character) {
        screen[currentX++] = blanck;
    }
    re_draw_line(start_position, currentX);
    currentX = i;
    lastX = currentX;
    set_cursor();
}

int8_t s_move_screen_up(uint8_t amount) {
    if(!amount) return 0;
    if(amount >= height) return -1;

    uint16_t displacement = amount * width, i;
    for(i = 0; i + displacement < MAX_SCREEN; i++){
        screen[i] = screen[i + displacement];
    }
    currentX -=displacement;
    lastX -=displacement;
    while (i < MAX_SCREEN) {
        screen[i++] = blanck;
    }
    re_draw_screen();
    return 0;
}

void print_s_char(s_char c) {
    printChar(c.character, currentX % width, currentX / width, c.fg_colour, c.bg_colour);
}

void s_off_cursor(){
    print_s_char(screen[currentX]);
}

void s_move_cursor(int8_t amount) {
    if (currentX + amount < 0 || currentX + amount > lastX) {
        return;
    }

    printChar(screen[currentX].character, currentX % width, currentX / width, screen[currentX].fg_colour, screen[currentX].bg_colour);
    currentX += amount;
    printChar(screen[currentX].character, currentX % width, currentX / width, BLACK, WHITE);
}

void s_insert_char(char c){
    if (lastX >= width * height - 1) {
        s_move_screen_up(1);
    }
    uint16_t aux = currentX;
    currentX = lastX;
    lastX++;
    while (currentX > aux) {
        screen[currentX] = screen[currentX - 1];
        print_s_char(screen[currentX]);
        currentX--;
    }
    screen[currentX].character = c;
    screen[currentX].fg_colour = WHITE;
    screen[currentX].bg_colour = BLACK;
    print_s_char(screen[currentX]);
    s_move_cursor(1);
}

void s_remove_char(){ 
    uint16_t aux = --currentX;
    while (currentX <= lastX) {
        screen[currentX] = screen[currentX + 1];
        print_s_char(screen[currentX]);
        currentX++;
    }
    lastX--;
    currentX = aux;
    set_cursor();
}

void s_clear(){
    for (uint16_t i = 0; i < MAX_SCREEN; i++) {
        screen[i] = blanck;
    }
    re_draw_screen();
    currentX = 0;
    lastX = 0;
}