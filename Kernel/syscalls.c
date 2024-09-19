#include <syscalls.h>
#include <time.h>
#include <audioDriver.h>
#include <keyboardDriver.h>
#include <videoDriver.h>

static uint32_t uintToBase(uint64_t value, uint8_t *buffer, uint32_t base);

uint32_t read(uint8_t *buffer, uint32_t size)
{
	uint32_t i = 0;
	uint8_t c;
	while (i < size && (c = nextFromBuffer()))
	{
		buffer[i++] = c;
	}
	return i;
}

static uint64_t lastCharX = 0;
static uint64_t lastCharY = 0;
long int write(FD fd, const uint8_t *string, uint32_t size)
{
	uint32_t height = getHeight() / 16;
	uint32_t width = getWidth();
	uint32_t colour = fd == STDOUT ? WHITE : RED;
	int i = 0;
	while (i < size)
	{
		if (string[i] == '\n' || (lastCharX + 1) * 8 == width)
		{
			lastCharY++;
			lastCharX = 0;
		}
		if (string[i] != '\n')
			drawchar(string[i], lastCharX++, lastCharY % height, colour, BLACK);
		i++;
	}
	return i;
}

void printRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			putPixel(color, x + i, y + j);
		}
	}
}

uint64_t getScreenWidth()
{
	return getWidth();
}

uint64_t getScreenHeight()
{
	return getHeight();
}

void printChar(uint8_t c, uint32_t x, uint32_t y, uint32_t fgcolor, uint32_t bgcolor)
{
	drawchar(c, x, y, fgcolor, bgcolor);
}

void wait(uint32_t tick)
{
	timer_wait(tick);
}

uint8_t keyboardStatus()
{
	return getKeyboardStatus();
}

uint8_t keyboardKey()
{
	return getKeyboardKey();
}

void beepSyscall(uint32_t frequence, uint32_t waitTicks)
{
	beep(frequence, waitTicks);
}

uint8_t fontSizeUp()
{
	return fontSizeBigger();
}

uint8_t fontSizeDown()
{
	return fontSizeSmaller();
}

void getTime(uint8_t pb[])
{
	uint8_t buff[64] = {'0'};
	uint8_t t;
	uint8_t idx = 0;
	for (uint8_t i = 2; i >= 0; i--)
	{
		t = getCurrentTime(i);
		if (i == 2)
		{
			if (t <= 2)
			{
				t += 0x24;
			}
			if (0x20 <= t && t <= 0x22)
			{
				t -= 0x6;
			}
			t -= 0x03;
		}
		if (t % 10 == t)
		{
			pb[idx] = '0';
			idx++;
		}
		uintToBase(t, buff, 16);
		int j;

		for (j = 0; buff[j] != 0; j++)
		{
			pb[idx] = buff[j];
			idx++;
		}
		if (i > 0)
		{
			pb[idx] = ':';
			idx++;
		}
	}
	pb[idx] = 0;
}

static uint32_t uintToBase(uint64_t value, uint8_t *buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	// Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	} while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	// Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}
