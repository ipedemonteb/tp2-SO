#include "../include/command_interpreter.h"
#include "../include/syscall.h"
#include "../include/libc.h"
#include "../include/shell_graphics.h"
#include "../include/tests.h"

#define LETTERS 'z' - 'a' + 1
#define WORDS 5

typedef struct command_t{
    char * name;
    void (*function)(uint8_t, char **);
} command_t;

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

char command[MAX_COMMAND];
int16_t command_length = 0;
int16_t fg_pid = -1;

static char * commandNotFoundMsg = "Command not found. Type help for a list of commands";
static char * helpMsg = "List of commands: clear, div0, eliminator, exit, fontBig, fontSmall, getTime, help, invalidOpCode";

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
    char * command_tokens[MAX_COMMAND/2] , * token, * command = command;
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
            int cmp = strcmp(command, auxC[j].name);
            if (cmp < 0) {
                break;
            }
            else if (cmp == 0) {
                fg_pid = create_process(auxC[j].function,i - 2,&command_tokens[1] , auxC[j].name);
                wait_pid(fg_pid, BLOCK);
                return;
            }
        }
        else {
            break;
        }
    }
    s_draw_line(commandNotFoundMsg, 0, 1);
}

void command_interpreter(){
    uint8_t terminal_pipe[2];
    pipe(INTERPRETER_PIPE_ID, terminal_pipe);
    while(1) {
        s_draw_line("", 1, 1);
        command_length = read(terminal_pipe[READ_END], command, MAX_COMMAND - 1);
        command[command_length] = 0;
        if(command[command_length - 1] == '\n')
            check_command();
    }

}

// comandos de la terminal

void fontBig(uint8_t argc, char * argv[]) {
    s_increase_font_size();
}

void fontSmall(uint8_t argc, char * argv[]) {
    s_decrease_font_size();
}

void exit(uint8_t argc, char * argv[]) {

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
    write(STDOUT, helpMsg, 97);
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