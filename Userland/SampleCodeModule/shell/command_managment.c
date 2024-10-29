#include "../include/command_manager.h"

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