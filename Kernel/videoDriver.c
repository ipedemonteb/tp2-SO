#include <videoDriver.h>
#include <font.h>
// tamaño de la ventana emergente
#define POP_UP_WIDTH 712
#define POP_UP_HEIGHT 256
#define BYTES_PER_PIXEL 3

// area en bytes de la ventana
#define POP_UP_AREA POP_UP_HEIGHT *POP_UP_WIDTH *BYTES_PER_PIXEL

// posicion del vertice superior izquierdo de la ventana emergente
#define TOP_LEFT_X 160
#define TOP_LEFT_Y 256

#define TOP_MARGIN 1
#define LEFT_MARGIN 2

uint8_t popUpWindowSave[POP_UP_AREA];

extern const unsigned char __font_bitmap__[];

VBEInfoPtr VBE_mode_info = (VBEInfoPtr)0x0000000000005C00;

// agregar (o no) font_size
uint8_t font_size = 1;

// imprime en la ventana emergente el mensaje desde la posicion relativa (x,y)
void printInPopUp(uint8_t *msg, uint16_t x, uint16_t y)
{
	uint8_t aux = font_size;
	font_size = 1;
	drawString(msg, x + TOP_LEFT_X / 8 + LEFT_MARGIN, y + TOP_LEFT_Y / 16 + TOP_MARGIN, WHITE, BLACK);
	font_size = aux;
}

void clearPopUp()
{
	uint8_t *framebuffer = (uint8_t *)VBE_mode_info->framebuffer;
	uint64_t offset = (TOP_LEFT_X * ((VBE_mode_info->bpp) / 8)) + (TOP_LEFT_Y * VBE_mode_info->pitch);
	uint64_t width = getWidth() * BYTES_PER_PIXEL;
	for (uint32_t i = 0; i < POP_UP_HEIGHT; i++)
	{
		for (uint32_t j = 0; j < POP_UP_WIDTH * BYTES_PER_PIXEL; j += 3)
		{
			if (i < 2 || i > POP_UP_HEIGHT - 3 || j < 2 * BYTES_PER_PIXEL || j > (POP_UP_WIDTH - 3) * BYTES_PER_PIXEL)
			{
				framebuffer[offset + i * width + j] = 0xCC;
				framebuffer[offset + i * width + j + 1] = 0xCC;
				framebuffer[offset + i * width + j + 2] = 0xCC;
			}
			else
			{
				framebuffer[offset + i * width + j] = 0x00;
				framebuffer[offset + i * width + j + 1] = 0x00;
				framebuffer[offset + i * width + j + 2] = 0x00;
			}
		}
	}
}

void resetPopUp()
{
	uint8_t *framebuffer = (uint8_t *)VBE_mode_info->framebuffer;
	uint64_t offset = (TOP_LEFT_X * ((VBE_mode_info->bpp) / 8)) + (TOP_LEFT_Y * VBE_mode_info->pitch);
	uint64_t width = getWidth() * BYTES_PER_PIXEL;

	for (uint32_t i = 0; i < POP_UP_HEIGHT; i++)
	{
		for (uint32_t j = 0; j < POP_UP_WIDTH * BYTES_PER_PIXEL; j += 3)
		{
			framebuffer[offset + i * width + j] = popUpWindowSave[i * POP_UP_WIDTH * BYTES_PER_PIXEL + j];
			framebuffer[offset + i * width + j + 1] = popUpWindowSave[i * POP_UP_WIDTH * BYTES_PER_PIXEL + j + 1];
			framebuffer[offset + i * width + j + 2] = popUpWindowSave[i * POP_UP_WIDTH * BYTES_PER_PIXEL + j + 2];
		}
	}
}

void savePopUpWindow()
{
	uint8_t *framebuffer = (uint8_t *)VBE_mode_info->framebuffer;
	uint64_t offset = (TOP_LEFT_X * ((VBE_mode_info->bpp) / 8)) + (TOP_LEFT_Y * VBE_mode_info->pitch);
	uint64_t width = getWidth() * BYTES_PER_PIXEL;
	for (uint32_t i = 0; i < POP_UP_HEIGHT; i++)
	{
		for (uint32_t j = 0; j < POP_UP_WIDTH * BYTES_PER_PIXEL; j += 3)
		{
			popUpWindowSave[i * POP_UP_WIDTH * BYTES_PER_PIXEL + j] = framebuffer[offset + i * width + j];
			popUpWindowSave[i * POP_UP_WIDTH * BYTES_PER_PIXEL + j + 1] = framebuffer[offset + i * width + j + 1];
			popUpWindowSave[i * POP_UP_WIDTH * BYTES_PER_PIXEL + j + 2] = framebuffer[offset + i * width + j + 2];
		}
	}
}

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y)
{
	uint8_t *framebuffer = (uint8_t *)VBE_mode_info->framebuffer;
	uint64_t offset = (x * ((VBE_mode_info->bpp) / 8)) + (y * VBE_mode_info->pitch);
	framebuffer[offset] = (hexColor) & 0xFF;
	framebuffer[offset + 1] = (hexColor >> 8) & 0xFF;
	framebuffer[offset + 2] = (hexColor >> 16) & 0xFF;
}

unsigned char *fontArr = __font_bitmap__;

void drawchar(uint8_t c, uint32_t x, uint32_t y, uint32_t fgcolor, uint32_t bgcolor)
{
	x *= 8 * font_size;
	y *= 16 * font_size;
	uint32_t cx, cy;
	uint32_t mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};
	for (cy = 0; cy < 16 * font_size; cy += font_size)
	{
		for (cx = 0; cx < 8 * font_size; cx += font_size)
		{
			for (uint8_t i = 0; i < font_size; i++)
			{
				for (uint8_t j = 0; j < font_size; j++)
				{
					putPixel(__font_bitmap__[((int)c - 32) * 16 + cy / font_size] & mask[cx / font_size] ? fgcolor : bgcolor, x + cx + j, y + cy + i);
				}
			}
		}
	}
}

void drawString(const uint8_t *string, uint32_t x, uint32_t y, uint32_t fgcolor, uint32_t bgcolor)
{
	uint32_t w = x, h = y;
	while (*string != 0)
	{
		if ((w * 8) >= VBE_mode_info->width)
		{
			h++;
			w = 0;
		}
		drawchar(*string, w, h, fgcolor, bgcolor);
		w++;
		string++;
	}
}

uint64_t getWidth()
{
	return VBE_mode_info->width;
}

uint64_t getHeight()
{
	return VBE_mode_info->height;
}

uint8_t fontSizeBigger()
{
	if (font_size >= 5)
		return 0;
	font_size++;
	return 1;
}

uint8_t fontSizeSmaller()
{
	if (font_size <= 1)
		return 0;
	font_size--;
	return 1;
}