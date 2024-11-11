#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include <stdint.h>
#include "userLib.h"
#include "libc.h"
#include "syscall.h"
#include "test_util.h"
#include "phylo.h"

#define MAX_LINE 1024
#define CLEAR -33

void remove_letter(char * string, uint8_t idx);
void insert_letter(char * string, int8_t len, char letter, int8_t idx);
void block_process(uint8_t argc, char * argv[]);
void cat(uint8_t argc, char * argv[]);
void clearCmd(uint8_t argc, char * argv[]);
void div0(uint8_t argc, char * argv[]);
void filter(uint8_t argc, char * argv[]);
void help(uint8_t argc, char * argv[]);
void invalidOpCode(uint8_t argc, char * argv[]);
void kill_process(uint8_t argc, char * argv[]);
void loop(uint8_t argc, char * argv[]);
void mem(uint8_t argc, char * argv[]);
void move_up(uint8_t argc, char * argv[]);
void nice_process(uint8_t arc, char * argv[]);
void phylo(uint8_t argc, char * argv[]);
void printps(uint8_t argc, char * argv[]);
void getTime(uint8_t argc, char * argv[]);
void wc(uint8_t argc, char * argv[]);

#endif