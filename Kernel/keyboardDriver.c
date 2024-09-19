#include <keyboardDriver.h>
#include <lib.h>
#include <regsDump.h>

static uint8_t charBuffer[BUFF_SIZE];
static uint64_t last = 0;
static uint64_t next = 0;

extern const uint64_t regs[19];

char asccode[58][4] =
	{
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{'1', '!', '1', '!'},
		{'2', '@', '2', '@'},
		{'3', '#', '3', '#'},
		{'4', '$', '4', '$'},
		{'5', '%', '5', '%'},
		{'6', '^', '6', '^'},
		{'7', '&', '7', '&'},
		{'8', '*', '8', '*'},
		{'9', '(', '9', '('},
		{'0', ')', '0', ')'},
		{'-', '_', '-', '_'},
		{'=', '+', '=', '+'},
		{127, 127, 127, 127},
		{'\t', '\t', '\t', '\t'},
		{'q', 'Q', 'Q', 'q'},
		{'w', 'W', 'W', 'w'},
		{'e', 'E', 'E', 'e'},
		{'r', 'R', 'R', 'r'},
		{'t', 'T', 'T', 't'},
		{'y', 'Y', 'Y', 'y'},
		{'u', 'U', 'U', 'u'},
		{'i', 'I', 'I', 'i'},
		{'o', 'O', 'O', 'o'},
		{'p', 'P', 'P', 'p'},
		{'[', '{', '[', '{'},
		{']', '}', ']', '}'},
		{'\n', '\n', '\n', '\n'},
		{0, 0, 0, 0},
		{'a', 'A', 'A', 'a'},
		{'s', 'S', 'S', 's'},
		{'d', 'D', 'D', 'd'},
		{'f', 'F', 'F', 'f'},
		{'g', 'G', 'G', 'g'},
		{'h', 'H', 'H', 'h'},
		{'j', 'J', 'J', 'j'},
		{'k', 'K', 'K', 'k'},
		{'l', 'L', 'L', 'l'},
		{';', ':', ';', ':'},
		{39, 34, 39, 34},
		{'`', '~', '`', '~'},
		{0, 0, 0, 0},
		{'\\', '|', '\\', '|'},
		{'z', 'Z', 'Z', 'z'},
		{'x', 'X', 'X', 'x'},
		{'c', 'C', 'C', 'c'},
		{'v', 'V', 'V', 'v'},
		{'b', 'B', 'B', 'b'},
		{'n', 'N', 'N', 'n'},
		{'m', 'M', 'M', 'm'},
		{',', '<', ',', '<'},
		{'.', '>', '.', '>'},
		{'/', '?', '/', '?'},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{' ', ' ', ' ', ' '},
};

char shiftOn = 0;
char ctrlOn = 0;
char extendedKey = 0;
char capsLock = 0;

// rutina de atencion de la interrupcion 21h
void keyboard_handler()
{
	uint8_t keyVal = getKeyboardKey();

	if (keyVal == 0xE0)
	{
		extendedKey = 1;
		keyVal = getKeyboardKey();
	}

	if (keyVal == LEFT_SHIFT || keyVal == RIGHT_SHIFT)
		shiftOn = 1;
	else if (keyVal == LEFT_SHIFT_B || keyVal == RIGHT_SHIFT_B)
		shiftOn = 0;
	else if (keyVal == LEFT_CTRL)
		ctrlOn = 1;
	else if (keyVal == LEFT_CTRL_B)
	{
		ctrlOn = 0;
	}
	else if (keyVal == CAPS_LOCK)
	{
		capsLock = !capsLock;
	}
	else if (extendedKey)
	{
		extendedKey = 0; // Reset the extended key flag
		uint8_t aux;
		switch (keyVal)
		{
		case DOWNKEY:
			aux = DOWN_ARROW;
			break;
		case UPKEY:
			aux = UP_ARROW;
			break;
		case RIGHTKEY:
			aux = RIGHT_ARROW;
			break;
		case LEFTKEY:
			aux = LEFT_ARROW;
			break;
		default:
			return;
		}
		charBuffer[last % BUFF_SIZE] = aux;
		last++;
	}
	else if (keyVal > 58)
	{
	}
	else
	{
		if (ctrlOn && keyVal == R_MAKE_CODE)
		{
			printRegisters("REGISTERS", regs);
			ctrlOn = 0;
		}
		else
		{
			charBuffer[last % BUFF_SIZE] = asccode[keyVal][shiftOn + capsLock * 2];
			last++;
		}
	}
}
uint8_t nextFromBuffer()
{
	return last <= next ? 0 : charBuffer[(next++) % BUFF_SIZE];
}