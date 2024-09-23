#ifndef REGSDUMP_H
#define REGSDUMP_H

#include <stdint.h>

static const uint8_t * registers[19] = {
    "RAX:",
    "RBX:",
    "RCX:",
    "RDX:",
    "RSI:",
    "RDI:",
    "RSP:",
    "RBP:",
    "R8 :",
    "R9 :",
    "R10:",
    "R11:",
    "R12:",
    "R13:",
    "R14:",
    "R15:",
    "RIP:",
    "CS :",
    "RFLAGS:",
};

void printRegisters(uint8_t * message, const uint64_t regs[19]);

#endif