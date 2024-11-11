#include "../include/keyboardDriver.h"
#include "../include/regsDump.h"
#include "../include/pipes.h"
#include "../include/terminal_driver.h"
#include <stdint.h>

static pipe_t * keyboard_buffer;
static pipe_t * terminal_buffer;
static uint8_t to_terminal = 0;

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
		{DELETE, DELETE, DELETE, DELETE},
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
		{'d', 'D', CTRL_D, CTRL_D},
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
		{'c', 'C', CTRL_C, CTRL_C},
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
void keyboard_handler() {
	uint8_t keyVal = getKeyboardKey();
	if (keyVal == 0xE0) {
		extendedKey = 1;
		keyVal = getKeyboardKey();
	}
	if (keyVal == LEFT_SHIFT || keyVal == RIGHT_SHIFT) {
		shiftOn = 1;
	}
	else if (keyVal == LEFT_SHIFT_B || keyVal == RIGHT_SHIFT_B) {
		shiftOn = 0;
	}
	else if (keyVal == LEFT_CTRL) {
		ctrlOn = 2;
	}
	else if (keyVal == LEFT_CTRL_B) {
		ctrlOn = 0;
	}
	else if (keyVal == CAPS_LOCK) {
		capsLock = !capsLock;
	}
	else if (extendedKey) {
		extendedKey = 0;
		char aux;
		switch (keyVal) {
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

		direct_write(keyboard_buffer, aux);
		keyboard_ready();
	}
	else if (keyVal <= 58) {
		if (ctrlOn && keyVal == R_MAKE_CODE) {
			printRegisters((int8_t *)"REGISTERS", regs);
			ctrlOn = 0;
		}
		else {
			char c = asccode[keyVal][(shiftOn ^ capsLock) + ctrlOn];
			if (c == CTRL_C) {
				t_insert_char('^');
				t_insert_char('C');
				t_kill_fg();
				t_off_cursor();
				t_draw_line("", 1);
				t_set_cursor();
				ctrlOn = 0;
				return;
			}
			if (c == CTRL_D) {
				t_insert_char('^');
				t_insert_char('D');
				t_off_cursor();
				t_draw_line("", 1);
				t_set_cursor();
			} else if (to_terminal) {
				if (c == '\n') {
					t_off_cursor();
					t_draw_line("", 1);
					t_set_cursor();
				} else if (c == DELETE) {
					t_remove_char();
					t_set_cursor();
				} else {
					t_insert_char(c);
				}
			}
			direct_write(keyboard_buffer, c);
			keyboard_ready();
		}
	}
}

void start_keyboard_driver(){
	keyboard_buffer = get_keyboard_buffer();
	terminal_buffer = get_terminal_buffer();
	keyboard_buffer->init = P_INIT;
	terminal_buffer->init = P_INIT;
}

void key_to_screen(uint8_t flag) {
	to_terminal = flag;
}