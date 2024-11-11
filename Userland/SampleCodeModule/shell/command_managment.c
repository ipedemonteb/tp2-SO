#include "../include/command_manager.h"

static char * helpMsg = "\
List of commands: \n\
- block: blocks the process with the given ID\n\
- cat: displays the content of the standard input\n\
- clear: clears the terminal \n\
- div0: tests div0 exception \n\
- filter: filters the input's vocals\n\
- help: displays all available commands\n\
- invalidOpCode: tests invalid opcode exception\n\
- kill: kills the process with the given ID\n\
- loop: prints the proccess ID with a greeting\n\
- mem: displays memory information\n\
- move_up: moves the screen the amout of lines received as a parameter\n\
- nice: changes the priority of the process with the given ID\n\
- phylo: starts the phylosophers problem\n\
- ps: lists all processes and their information\n\
- test_mm: tests memory management\n\
- test_prio: tests priority management\n\
- test_processes: tests process management\n\
- test_sync: tests semaphores\n\
- wc: counts the number of lines of the input\n\
";

void remove_letter(char * string, uint8_t idx) {
    while(string[idx]) {
        string[idx] = string[idx + 1];
        idx++;
    }
}

void insert_letter(char * string, int8_t len, char letter, int8_t idx) {
    while(len + 1 > idx){
        string[len + 1] = string[len];
        len--;
    }
    string[idx] = letter;
}

void block_process(uint8_t argc, char * argv[]) {
    if (argc != 1) {
        printf("Usage: block <pid>\n");
        return;
    }
    uint64_t pid = atoi(argv[0]);
    block(pid);
}

#define MAX_LINE 256

void cat(uint8_t argc, char * argv[]) {
    char c, buff[MAX_LINE];
    uint16_t count = 0;
    while ((c = get_char()) != -1) {
        buff[count++] = c;
        if (c == '\n' || count - 1 >= MAX_LINE ) {
            buff[count] = 0;
            printf(buff);
            count = 0;
        }
    }
}

//implement
void clearCmd(uint8_t argc, char * argv[]) {

}

void div0(uint8_t argc, char * argv[]) {
    divZero();
}

//delete
void eliminator(uint8_t argc, char * argv[]) {
    /* gameMain();
    cleanBuffer();
    sMoveScreenUp(0);
    reset = 1; */
}

//implement
void filter(uint8_t argc, char * argv[]) {

}

void help(uint8_t argc, char * argv[]) {
    printf("%s", helpMsg);
}

void invalidOpCode(uint8_t argc, char * argv[]) {
    opcode();
}

void kill_process(uint8_t argc, char * argv[]) {
    if (argc != 1) {
        printf("Usage: kill <pid>\n");
        return;
    }
    uint64_t pid = atoi(argv[0]);
    kill(pid);
}

void loop(uint8_t argc, char * argv[]) {
    while(1) {
        printf("Hello from process %d\n", get_current_pid());
        wait(100);
    }
}

void mem(uint8_t argc, char * argv[]) {
    mem_info * info = my_malloc(sizeof(mem_info));
    mem_data(info);
    printf("Total memory: %d B\n", info->total);
    printf("Used memory: %d B\n",  info->used);
    printf("Free memory: %d B\n",  info->free);
    my_free(info);
}

void move_up(uint8_t argc, char * argv[]) {
    if (argc != 1) {
        printf("Usage: move_up <amount[1-9]>\n");
        return;
    }
    int8_t amount = satoi(argv[0]);
    if (amount <= 0 || amount > 9) {
        printf("amount must be between 0 and 9\n");
        return;
    }
    put_char(amount);
}

//check and check max priority
void nice_process(uint8_t argc, char * argv[]) {
    if (argc != 2) {
        printf("Usage: nice <pid> <priority>\n");
        return;
    }
    uint64_t pid = atoi(argv[0]);
    uint8_t priority = atoi(argv[1]);
    nice(pid, priority);
}

void phylo(uint8_t argc, char * argv[]) {
    run_phylo();
}

void printps(uint8_t argc, char * argv[]) {
    process_info * info = my_malloc(sizeof(process_info) * 128);
    uint8_t process_count = ps(info);
    printf("PID     NAME         PRIORITY     STACK_BASE      STACK_PTR       FOREGROUND    STATUS   \n");
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
        my_free(info[i].name);
        jmp += 13;

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
        buffer[jmp + strlen(msg[index])] = 0;
        printf("%s\n", buffer);
    }
}

//check
void wc(uint8_t argc, char * argv[]) {
    char c;
    uint64_t count = 0;
    while (read(STDIN, &c, 1) > 0) {
        if (c == '\n') {
            count++;
        }
    }
    printf("%d\n", count);
}