#include <stdint.h>
#include "../include/interrupts.h"
#include "../include/regsDump.h"

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 6

static void zero_division(const uint64_t regs[19]) {
	printRegisters((int8_t *)"Division by zero exception", regs);
	return;
}

static void invalid_opcode(const uint64_t regs[19]) {
	printRegisters((int8_t *)"Invalid opcode exception", regs);
	return;
}

void exceptionDispatcher(uint8_t exception, const uint64_t regs[19]) {
	_sti();
	switch (exception) {
	case ZERO_EXCEPTION_ID:
		zero_division(regs);
		break;
	case INVALID_OPCODE_ID:
		invalid_opcode(regs);
		break;
	}
}