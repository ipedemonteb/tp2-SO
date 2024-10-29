#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include <stdint.h>

void remove_letter(char * string, uint8_t idx);

void insert_letter(char * string, int8_t len, char letter, int8_t idx);

#endif