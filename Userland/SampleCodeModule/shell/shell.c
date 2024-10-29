#include <stdint.h>
#include "../include/shell.h"
#include "../include/libc.h"
#include "../include/userLib.h"
#include "../include/sounds.h"
//#include "../include/eliminator.h"
#include "../include/syscall.h"
#include "../include/tests.h"
#include "../include/string_arrayADT.h"
#include "../include/shell_graphics.h"
#include "../include/command_manager.h"

#define BUFF_MAX 4096
#define WHITE 0x00FFFFFF
#define BLACK 0
#define DELETE 127
#define LEFT_ARROW 251
#define RIGHT_ARROW 252
#define UP_ARROW 253
#define DOWN_ARROW 254
#define LETTERS 'z' - 'a' + 1
#define WORDS 5
#define MAX_COMMAND 128

typedef struct command_t{
    char * name;
    void (*function)(uint8_t, char **);
} command_t;

string_arrayADT saved_commands;

char current_command[MAX_COMMAND] = {0};
uint16_t current_command_length = 0;
uint16_t current_command_pos = 0;

static void clearCmd(uint8_t argc, char * argv[]);
static void div0(uint8_t argc, char * argv[]);
static void exit(uint8_t argc, char * argv[]);
static void eliminator(uint8_t argc, char * argv[]);
static void fontSmall(uint8_t argc, char * argv[]);
static void fontBig(uint8_t argc, char * argv[]);
static void getTime(uint8_t argc, char * argv[]);
static void help(uint8_t argc, char * argv[]);
static void invalidOpCode(uint8_t argc, char * argv[]);
static void prio(uint8_t argc, char * argv[]);
static void processes(uint8_t argc, char * argv[]);
static void mm(uint8_t argc, char * argv[]);
static void printps(uint8_t argc, char * argv[]);
static void sync(uint8_t argc, char * argv[]);
static void move_up(uint8_t argc, char * argv[]);

static command_t commands[LETTERS][WORDS] = {
    {{0,0}},  //a
    {{0,0}},  //b
    {{"clear", clearCmd}, {0, 0}}, 
    {{"div0", div0}, {0, 0}}, 
    {{"eliminator", eliminator}, {"exit", exit}, {0, 0}}, 
    {{"fontBig", fontBig}, {"fontSmall", fontSmall}, {0, 0}}, 
    {{"getTime", getTime}, {0, 0}},
    {{"help", help}, {0, 0}}, 
    {{"invalidOpCode", invalidOpCode}, {0, 0}},
    {{0,0}},  //j
    {{0,0}},  //k
    {{0,0}},  //l
    {{"move_up", move_up},{0,0}},  //m
    {{0,0}},  //n
    {{0,0}},  //o
    {{"ps", printps},{0,0}},  //p
    {{0,0}},  //q
    {{0,0}},  //r
    {{0,0}},  //s
    {{"test_mm",mm},{"test_prio",prio},{"test_processes",processes},{"test_sync", sync}, {0,0}},   //t
    {{0,0}},  //u
    {{0,0}},  //v
    {{0,0}},  //x
    {{0,0}},  //y
    {{0,0}}  //z
};

static char * commandNotFoundMsg = "Command not found. Type help for a list of commands";
static char * helpMsg = "List of commands: clear, div0, eliminator, exit, fontBig, fontSmall, getTime, help, invalidOpCode";

static uint8_t exitFlag;

int8_t getCommandIdx(uint8_t c) {
    if (c <= 'Z') {
        return c - 'A';
    }
    if (c <= 'z')
    {
        return c - 'a';
    }
    return -1;
}

void check_command(){
    char * command_tokens[MAX_COMMAND/2] , * token, * command = current_command;
    uint8_t i = 0;
    do
    {
        token = strtok(command," ");
        command_tokens[i++] = token;
        command = NULL;
    } while (token);
    
    int8_t c = getCommandIdx(command_tokens[0][0]);
    if (c < 0) {
        s_draw_line(commandNotFoundMsg, 0, 1);
        return;
    }
    command_t * auxC = commands[c];
    for (int j = 0; j < WORDS; j++) {
        if (auxC[j].name != NULL) {
            int cmp = strcmp(current_command, auxC[j].name);
            if (cmp < 0) {
                break;
            }
            else if (cmp == 0) {
                auxC[j].function(i - 2,&command_tokens[1]);
                return;
            }
        }
        else {
            break;
        }
    }
    s_draw_line(commandNotFoundMsg, 0, 1);
}

void launchShell() {
    s_char prompt[] = {{'$', WHITE, BLACK},{'>', WHITE, BLACK}, {' ', WHITE, BLACK}};
    s_start_graphics(prompt, 3);
    char * empty = "";
    s_draw_line(empty, 1, 0);
    s_set_cursor();
    saved_commands = start_string_array(BUFF_MAX);
    uint8_t key;
    while (!exitFlag) {
        key = getChar();
        switch (key) {
            case RIGHT_ARROW:
                s_move_cursor(1);
                if(current_command_pos < current_command_length)
                    current_command_pos++;
                break;
            case LEFT_ARROW:
                if (current_command_pos > 0){
                    s_move_cursor(-1);
                    current_command_pos--;
                }
                break;
            case UP_ARROW:
                if (has_previous(saved_commands)) {
                    s_off_cursor();
                    strcpy(current_command,previous(saved_commands, &current_command_length));
                    current_command_pos = current_command_length;
                    s_draw_line(current_command, 1, 0);
                    s_set_cursor();
                }
                break;
            case DOWN_ARROW:
                if (has_next(saved_commands)) {
                    s_off_cursor();
                    strcpy(current_command,next(saved_commands, &current_command_length));
                    current_command_pos = current_command_length;
                    s_draw_line(current_command, 1, 0);
                    s_set_cursor();
                }
                break;
            case DELETE:
                if (current_command_pos > 0){
                    s_remove_char();
                    s_set_cursor();
                    current_command_pos--;
                    remove_letter(current_command, current_command_pos);
                    current_command_length--;
                }
                break;
            case '\n':
                current_command[current_command_length] = 0;
                s_off_cursor();
                if (current_command_length != 0) {
                    add(saved_commands, current_command, current_command_length);
                    to_begin(saved_commands);
                    check_command();
                    current_command[0] = 0;
                    current_command_length = 0;
                    current_command_pos = 0;
                }
                s_draw_line(empty, 1, 1);
                s_set_cursor();
                break;
            case '\t':
                int8_t index;
                if (current_command_length == 1 && (index  = getCommandIdx(current_command[0])) >= 0 && commands[index]->name) {
                    current_command_length = strcpy(current_command, commands[index]->name);
                    current_command_pos = current_command_length;
                    s_draw_line(current_command, 1, 0);
                    s_set_cursor();
                }
            case 0:
                break;
            default: 
                s_insert_char(key);
                insert_letter(current_command, current_command_length, key, current_command_pos);
                current_command_length++;
                current_command_pos++;
                break;
        }
    }
    while (s_decrease_font_size() != -1);
}

// comandos de la terminal

void fontBig(uint8_t argc, char * argv[]) {
    s_increase_font_size();
}

void fontSmall(uint8_t argc, char * argv[]) {
    s_decrease_font_size();
}

void exit(uint8_t argc, char * argv[]) {
    exitFlag = 1;
}

void div0(uint8_t argc, char * argv[]) {
    while (s_decrease_font_size() != -1);
    divZero();
}


void move_up(uint8_t argc, char * argv[]) {
    s_move_screen_up(satoi(argv[0]));
}

void eliminator(uint8_t argc, char * argv[]) {
    /* gameMain();
    cleanBuffer();
    sMoveScreenUp(0);
    reset = 1; */
}

void clearCmd(uint8_t argc, char * argv[]) {
    s_clear();
}

void getTime(uint8_t argc, char * argv[]) {
    char clock[20];
    time(clock);
    s_draw_line(clock, 0, 1);
}

void help(uint8_t argc, char * argv[]) {
    s_draw_line(helpMsg,0,1);
}

void invalidOpCode(uint8_t argc, char * argv[]) {
    while (s_decrease_font_size() != -1);
    opcode();
}

void sync(uint8_t argc, char * argv[]) {
    create_process(test_sync, argc, argv, "test_sync");
    wait_children();
}

void prio(uint8_t argc, char * argv[]){
    create_process(test_prio,0,0,"test_prio");
    wait_children();
}

void processes(uint8_t argc, char * argv[]){
    char * argv2[] = {"4", 0};
    create_process(test_processes,1, argv2,"test_processes");
    wait_children();
}


void mm(uint8_t argc, char * argv[]){
    create_process(test_mm,1,0,"test_mm");
    wait_children();
}

void printps(uint8_t argc, char * argv[]) {
    process_info * info = my_malloc(sizeof(process_info) * 128);
    uint8_t process_count = ps(info);
    char header[] = "PID     NAME         PRIORITY     STACK_BASE      STACK_PTR       FOREGROUND    STATUS   ";
    s_draw_line(header,0,1);
    for(int i = 0; i < process_count; i++) {
        uint8_t jmp = 0;
        char buffer[127];
        char * buffer_ptr = buffer;
        char aux[30];

        for(int i = 0; i < 127; i++) {
            buffer[i] = ' ';
        }

        //copy pid
        itos(info[i].pid, aux);
        strcpy(buffer, aux);
        buffer[strlen(aux)] = ' ';
        jmp += 8;

        //copy name
        strcpy(buffer + jmp, info[i].name);
        buffer[jmp + strlen(info[i].name)] = ' ';
        jmp += 13;
        my_free(info[i].name);

        //copy priority
        itos(info[i].priority, aux);
        strcpy(buffer + jmp, aux);
        buffer_ptr[jmp + strlen(aux)] = ' ';
        jmp += 13;

        //copy stack base
        uint64ToHexStr((uint64_t)info[i].stack_base, aux);
        strcpy(buffer + jmp, aux);
        buffer[jmp + strlen(aux)] = ' ';
        jmp += 16;

        //copy stack ptr
        uint64ToHexStr((uint64_t)info[i].stack_ptr, aux);
        strcpy(buffer + jmp, aux);
        buffer[jmp + strlen(aux)] = ' ';
        jmp += 16;
        
        //copy foreground
        itos(info[i].foreground, aux);
        strcpy(buffer + jmp, aux);
        buffer[jmp + strlen(aux)] = ' ';
        jmp += 14;

        //copy status
        char * msg[] = {"READY", "BLOCKED", "KILLED"};
        int index = 0;
        switch (info[i].status) {
            case BLOCKED:
                index = 1;
                break;
            case KILLED:
                index = 2;
                break;
            default:
                break;
        }
        strcpy(buffer + jmp, msg[index]);
        buffer[jmp + strlen(msg[index])] = ' ';

        buffer[127] = 0;
        s_draw_line(buffer,0,1);
    }

}