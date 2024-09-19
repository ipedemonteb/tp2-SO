/* sampleCodeModule.c */
#include <shell.h>
#include <libc.h>
#include <eliminator.h>
#include <syscaller.h>
#include <stdint.h>
#include <welcome.h>
#include <font.h>

int main()
{
	while (1)
	{
		welcome();
		uint8_t *msg2[] = {"Please select a program to run:\0", "1. Eliminator\0", "2. Shell\0"};
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
	}
}
