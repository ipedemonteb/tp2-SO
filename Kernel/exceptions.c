#include <stdint.h>
#include <syscalls.h>
#include <interrupts.h>
#include <regsDump.h>
#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 6

static void zero_division();
static void invalid_opcode();

void exceptionDispatcher(uint8_t exception, const uint64_t regs[19])
{
	_sti();
	switch (exception)
	{
	case ZERO_EXCEPTION_ID:
		zero_division(regs);
		break;
	case INVALID_OPCODE_ID:
		invalid_opcode(regs);
		break;
	}
}

static void zero_division(const uint64_t regs[19])
{
	printRegisters("Division by zero exception", regs);
	return;
}

static void invalid_opcode(const uint64_t regs[19])
{
	printRegisters("Invalid opcode exception", regs);
	return;
}
