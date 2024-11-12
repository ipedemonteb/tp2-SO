#include "../include/shell.h"

#define BUFF_MAX 4096
#define WHITE 0x00FFFFFF
#define BLACK 0
#define DELETE 127
#define LEFT_ARROW -5
#define RIGHT_ARROW -4
#define UP_ARROW -3
#define DOWN_ARROW -2
#define LETTERS 'z' - 'a' + 1
#define WORDS 5
#define MAX_COMMAND 128
#define MAX_PID 127
#define FG 1
#define BG 2

typedef struct command_t{
    char * name;
    void (* function)(uint8_t, char **);
} command_t;

string_arrayADT saved_commands;

char current_command[MAX_COMMAND] = {0};
uint16_t current_command_length = 0;
uint16_t current_command_pos = 0;

char * prompt = "$> ";
uint8_t prompt_len = 3;

static command_t commands[LETTERS][WORDS] = {
    {{0,0}},  //a
    {{"block", block_process}, {0,0}},  //b
    {{"cat", cat},{"clear", clearCmd}, {0, 0}}, //c
    {{"div0", div0}, {0, 0}}, //d
    {{0, 0}}, //e
    {{"filter", filter}, {0, 0}}, //f
    { {0, 0}}, //g
    {{"help", help}, {0, 0}}, //h
    {{"invalidOpCode", invalidOpCode}, {0, 0}}, //i
    {{0,0}},  //j
    {{"kill", kill_process}, {0,0}},  //k
    {{"loop",loop}, {0,0}},  //l
    {{"mem", mem}, {"move_up", move_up},{0,0}},  //m
    {{"nice", nice_process}, {0,0}},  //n
    {{0,0}},  //o
    {{"phylo", phylo}, {"ps", printps},{0,0}},  //p
    {{0,0}},  //q
    {{0,0}},  //r
    {{0,0}},  //s
    {{"test_mm",test_mm},{"test_prio",test_prio},{"test_processes",test_processes},{"test_sync", test_sync}, {0,0}},   //t
    {{0,0}},  //u
    {{0,0}},  //v
    {{"wc",wc}, {0,0}}, //w
    {{0,0}},  //x
    {{0,0}},  //y
    {{0,0}}  //z
};


static uint8_t exitFlag;

uint8_t running_processes_pids[MAX_PID + 1] = {0};

int8_t getCommandIdx(uint8_t c) {
    if (c <= 'Z') {
        return c - 'A';
    }
    if (c <= 'z') {
        return c - 'a';
    }
    return -1;
}

command_t * find_command(char * token) {
    int8_t c = getCommandIdx(token[0]);
    if (c < 0) {
        return NULL;
    }
    command_t * command_array = commands[c];
    for (int j = 0; j < WORDS; j++) {
        if (command_array[j].name != NULL) {
            int cmp = strcmp(token, command_array[j].name);
            if (cmp < 0) {
                break;
            }
            else if (cmp == 0) {
                return command_array + j;
            }
        }
        else {
            break;
        }
    }
    return NULL;
}

uint8_t get_tokens(char ** tokens, char * source, const char * delim) {
    char * token;
    uint8_t i = 0;
    do
    {
        token = strtok(source,delim);
        tokens[i++] = token;
        source = NULL;
    } while (token);
    tokens[i] = NULL;
    return i;
}

void wait_for_processes(uint8_t block) {
     for (uint8_t i = 0; i < MAX_PID + 1; i++) {
        uint8_t state = running_processes_pids[i];
        if (state) {
            if(wait_pid(i, block & state)) {
                running_processes_pids[i] = 0;
                if (!block) {
                    printf("[%d]Done\n",i);
                }
                
            }
        }
    }
}

uint8_t check_bg() {
    while (current_command[current_command_length - 1] == ' ') {
        current_command[current_command_length - 1] = 0;
        current_command_length--;
    }
    if(current_command[current_command_length - 1] == '&'){
        current_command[current_command_length - 1] = 0;
        current_command_length--;
        return 1;
    }
    return 0;
}

void check_command(){
    uint8_t bg = check_bg();
    key_to_screen(1);
    char * commands[MAX_COMMAND/2], * command_tokens[MAX_COMMAND/2];
    uint8_t command_count = get_tokens(commands, current_command, "|") - 1;

    if (command_count > 2) {
        write(TERMINAL, "Only one pipe supported.\n",25);
        key_to_screen(0);
        return;
    }
    
    uint8_t command_argc;
    command_t * c;

    uint8_t p[2];
    pipe(ANON,p);
    if (command_count == 2) {
        swap(STDIN,p[READ_END]);

        command_argc = get_tokens(command_tokens, commands[1], " ") - 2;
        c = find_command(command_tokens[0]);
        if (c == NULL) {
            printf("%s: Command not found. Type help for a list of commands\n", command_tokens[0]);
        } else {
            uint16_t pid = create_process(c->function,command_argc, command_tokens + 1, c->name, !bg);
            running_processes_pids[pid] = 1 + bg;
            nice(pid, bg ? 3 : 4);
        }
        swap(STDIN, p[READ_END]);
    } else {
        copy(p[WRITE_END],STDOUT);
    }
    swap(STDOUT, p[WRITE_END]);
    
    command_argc = get_tokens(command_tokens, commands[0], " ") - 2;
    c = find_command(command_tokens[0]);
    if (c == NULL) {
        printf("%s: Command not found. Type help for a list of commands\n", command_tokens[0]);
    } else { 
        uint16_t pid = create_process(c->function,command_argc, command_tokens + 1, c->name, !bg);
        running_processes_pids[pid] = 1 + bg;
        nice(pid, bg ? 3 : 4);
    }
    swap(STDOUT, p[WRITE_END]);
    close(p[WRITE_END]);
    close(p[READ_END]);

    wait_for_processes(!bg);
    key_to_screen(0);    
}

void handle_up_down(int8_t (*condition)(string_arrayADT), char * (*fn)(string_arrayADT,uint16_t *), char * key) {
    if(condition(saved_commands)) {
        write(TERMINAL, key, 1);
        strcpy(current_command,fn(saved_commands, &current_command_length));
        current_command_pos = current_command_length;
        write(TERMINAL, prompt, prompt_len);
        write(TERMINAL, current_command, current_command_length);
    }
}

void launchShell() {
    copy(STDOUT,TERMINAL);
    nice(get_current_pid(), 4);
    write(TERMINAL, prompt, 3);
    saved_commands = start_string_array(BUFF_MAX);
    char key;
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
                wait_for_processes(NO_BLOCK);
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