/* sampleCodeModule.c */
#include <stdint.h>
#include "./include/shell.h"
#include "./include/font.h"
#include "./include/syscall.h"

void inactive() {
	while (1) {//@todo: ver de cammbiar por un hlt
		yield();
	}
	
}

int main() {
	char * argv[] = {0};
	create_process((void *)inactive, 0 , argv, "inactive");
	create_process((void *)launchShell, 0, argv,"Shell");
	wait_children();
	while (1) {
		/* code */
	}
	
	/* while (1)
	{
		//welcome();
		int8_t *msg2[] = {(int8_t *)"Please select a program to run:\0", (int8_t *)"1. Eliminator\0", (int8_t *)"2. Shell\0"};
		for (uint8_t i = 0; i < 3; i++)
		{
			uint8_t j = 0;
			while (msg2[i][j])
			{
				drawBigChar(msg2[i][j], 82 + j, 2 + i, ORANGE, WHITE);
				j++;
			}
		}
		uint8_t c = 0;
		while (c != '1' && c != '2')
			readCaller(UNUSED, &c, 1);
		switch (c)
		{
		case '1':
			gameMain();
			break;

		case '2':
			launchShell();
			break;
		}
	} */
}