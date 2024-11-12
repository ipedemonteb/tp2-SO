// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/regsDump.h"
#include "../include/keyboardDriver.h"
#include "../include/videoDriver.h"
#include <stdint.h>

static const int8_t * registers[19] = {
   (const int8_t *)"RAX:",
    (const int8_t *)"RBX:",
    (const int8_t *)"RCX:",
    (const int8_t *)"RDX:",
    (const int8_t *)"RSI:",
    (const int8_t *)"RDI:",
    (const int8_t *)"RSP:",
    (const int8_t *)"RBP:",
    (const int8_t *)"R8 :",
    (const int8_t *)"R9 :",
    (const int8_t *)"R10:",
    (const int8_t *)"R11:",
    (const int8_t *)"R12:",
    (const int8_t *)"R13:",
    (const int8_t *)"R14:",
    (const int8_t *)"R15:",
    (const int8_t *)"RIP:",
   (const int8_t *) "CS :",
    (const int8_t *)"RFLAGS:",
};

void uint64ToHex(uint64_t num, int8_t buf[17]) {
    int i = 15;
    while (i >= 0) {
        uint32_t digit = num % 16;
        buf[i] = (digit < 10 ? '0' : ('A' - 10)) + digit;
        num /= 16;
        i--;
    }
    buf[16] = 0;
}

void printRegisters(int8_t * msg, const uint64_t regs[19]) {
    savePopUpWindow();
    clearPopUp();
    printInPopUp(msg, 0, 0);
    int8_t buf[19];
    buf[0] = '0';
    buf[1] = 'x';
    for (uint8_t i = 0; i < 18; i++) {
        printInPopUp((int8_t *)registers[i], (i % 2) * 24, i / 2 + 1);
        uint64ToHex(regs[i], buf + 2);
        printInPopUp(buf, (i % 2) * 24 + 5, i / 2 + 1);
    }
    printInPopUp((int8_t *)registers[18], 0, 10);
    uint64ToHex(regs[18], buf + 2);
    printInPopUp(buf, 8, 10);
    printInPopUp((int8_t *)"Press ESC to continue", 0, 11);
    while (getKeyboardKey() != ESC_MAKE_CODE) {
        ;
    }
    resetPopUp();
}