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
#define TERMINAL 3

typedef struct command_t{
    char * name;
    void (*function)(uint8_t, char **);
} command_t;

string_arrayADT saved_commands;

char current_command[MAX_COMMAND] = {0};
uint16_t current_command_length = 0;
uint16_t current_command_pos = 0;

char * prompt = "$> ";
uint8_t prompt_len = 3;

static void clearCmd(uint8_t argc, char * argv[]);
static void div0(uint8_t argc, char * argv[]);
static void exit(uint8_t argc, char * argv[]);
static void eliminator(uint8_t argc, char * argv[]);
static void getTime(uint8_t argc, char * argv[]);
static void help(uint8_t argc, char * argv[]);
static void invalidOpCode(uint8_t argc, char * argv[]);
static void printps(uint8_t argc, char * argv[]);
static void sleep(uint8_t argc, char * argv[]);
static void move_up(uint8_t argc, char * argv[]);

static command_t commands[LETTERS][WORDS] = {
    {{0,0}},  //a
    {{0,0}},  //b
    {{"clear", clearCmd}, {0, 0}}, 
    {{"div0", div0}, {0, 0}}, 
    {{"eliminator", eliminator}, {"exit", exit}, {0, 0}}, 
    {{0, 0}}, 
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
    {{"sleep", sleep},{0,0}},  //s
    {{"test_mm",test_mm},{"test_prio",test_prio},{"test_processes",test_processes},{"test_sync", test_sync}, {0,0}},   //t
    {{0,0}},  //u
    {{0,0}},  //v
    {{0,0}},  //x
    {{0,0}},  //y
    {{0,0}}  //z
};

static char * command_not_found_msg = "Command not found. Type help for a list of commands\n";
static char * helpMsg = "List of commands: clear, div0, eliminator, exit, getTime, help, invalidOpCode";

static uint8_t exitFlag;

void (*to_launch)(uint8_t, char **);
uint8_t stdin;
uint8_t stdout;

void command_launcher(uint8_t argc, char * argv[]) {
    copy(STDIN, stdin);
    copy(STDOUT, stdout);
    close(stdin);
    close(stdout);
    to_launch(argc, argv);
}

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
        write(TERMINAL, command_not_found_msg, 52); //@todo: cambiar los write por printf o algo asi
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
                stdin = STDIN;
                stdout = TERMINAL;
                to_launch = auxC[j].function;
                key_to_screen(1);
                wait_pid(create_process(command_launcher,i - 2,&command_tokens[1], auxC[j].name), BLOCK);
                key_to_screen(0);
                write(TERMINAL, "\n", 1);
                return;
            }
        }
        else {
            break;
        }
    }
    write(TERMINAL, command_not_found_msg, 52); //@todo: cambiar los write por printf o algo asi
}

void handle_up_down(int8_t (*condition)(string_arrayADT), char * (*fn)(string_arrayADT,uint16_t *), uint8_t * key) {
    if(condition(saved_commands)) {
        write(TERMINAL, key, 1);
        strcpy(current_command,fn(saved_commands, &current_command_length));
        current_command_pos = current_command_length;
        write(TERMINAL, prompt, prompt_len);
        write(TERMINAL, current_command, current_command_length);
    }
}

void launchShell() {
    write(TERMINAL, prompt, 3);
    saved_commands = start_string_array(BUFF_MAX);
    uint8_t key;
    while (!exitFlag) {
        key = get_char();
        switch (key) {
            case RIGHT_ARROW:
                if(current_command_pos < current_command_length) {
                    write(TERMINAL, &key, 1);
                    current_command_pos++;
                }
                break;
            case LEFT_ARROW:
                if (current_command_pos > 0){
                    write(TERMINAL, &key, 1);
                    current_command_pos--;
                }
                break;
            case UP_ARROW:
                handle_up_down(has_previous, previous, &key);
                break;
            case DOWN_ARROW:
                handle_up_down(has_next, next, &key);
                break;
            case DELETE:
                if (current_command_pos > 0){
                    write(TERMINAL, &key, 1); //@todo: esta muchas veces repetido esto
                    current_command_pos--;
                    remove_letter(current_command, current_command_pos);
                    current_command_length--;
                }
                break;
            case '\n':
                write(TERMINAL, &key, 1);
                current_command[current_command_length] = 0;
                if (current_command_length != 0) {
                    add(saved_commands, current_command, current_command_length);
                    to_begin(saved_commands);
                    check_command();
                    current_command[0] = 0;
                    current_command_length = 0;
                    current_command_pos = 0;
                }
                write(TERMINAL, prompt, prompt_len);
                break;
            case '\t':
                int8_t index;
                if (current_command_length == 1 && (index  = getCommandIdx(current_command[0])) >= 0 && commands[index]->name) {
                    current_command_length = strcpy(current_command, commands[index]->name);
                    current_command_pos = current_command_length;
                    write(TERMINAL, current_command + 1, current_command_length - 1);
                }
            case 0:
                break;
            default: 
                write(TERMINAL, &key, 1);
                insert_letter(current_command, current_command_length, key, current_command_pos);
                current_command_length++;
                current_command_pos++;
                break;
        }
    }
}

// comandos de la terminal

void exit(uint8_t argc, char * argv[]) {

}

void div0(uint8_t argc, char * argv[]) {

    divZero();
}


void move_up(uint8_t argc, char * argv[]) {

}

void eliminator(uint8_t argc, char * argv[]) {
    /* gameMain();
    cleanBuffer();
    sMoveScreenUp(0);
    reset = 1; */
}

void clearCmd(uint8_t argc, char * argv[]) {

}

void getTime(uint8_t argc, char * argv[]) {
    //char clock[20];
    //time(clock);

}

void help(uint8_t argc, char * argv[]) {
    printf(helpMsg);
}

void sleep(uint8_t argc, char * argv[]) {
    bussy_wait(1000000000);
}

void invalidOpCode(uint8_t argc, char * argv[]) {

    opcode();
}

void printps(uint8_t argc, char * argv[]) {
    process_info * info = my_malloc(sizeof(process_info) * 128);
    uint8_t process_count = ps(info);
    char header[] = "PID     NAME         PRIORITY     STACK_BASE      STACK_PTR       FOREGROUND    STATUS   \n";
    write(STDOUT, header, 90);
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
        buffer[jmp + strlen(msg[index])] = '\n';
        write(STDOUT, buffer, jmp + strlen(msg[index]) + 1);
    }
}