#include "../include/command_manager.h"

void remove_letter(char * string, uint8_t idx) {
        while(string[idx]) {
            string[idx] = string[idx + 1];
            idx++;
        }
}

void insert_letter(char * string, uint8_t len, char letter, uint8_t idx) {
    while(len > idx){
        string[len + 1] = string[len];
        len--;
    }
    string[idx] = letter;
}