#ifndef REGSDUMP_H
#define REGSDUMP_H

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

void printRegisters(int8_t * message, const uint64_t regs[19]);

#endif