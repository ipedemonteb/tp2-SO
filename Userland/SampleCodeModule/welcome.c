#include <welcome.h>
#include <syscaller.h>
#include <font.h>

void welcome()
{
    fontSizeDownCaller(UNUSED);
    printRectangleCaller(UNUSED, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK); // set screen black
    uint16_t startX = 156;
    int startY = 160;
    uint32_t t = 0;

    for (uint32_t i = 0; i < PENGUIN_HEIGHT + 6; i++)
    {
        for (uint32_t j = 0; j < PENGUIN_WIDTH + 6; j++)
        {
            if (i < 3 || i >= PENGUIN_HEIGHT + 3 || j < 3 || j >= PENGUIN_WIDTH + 3)
            {
                printRectangleCaller(UNUSED, j * 5 + startX, i * 5 + startY, 5, 5, WHITE);
            }
            else
            {
                uint32_t color = penguin[t++];
                printRectangleCaller(UNUSED, j * 5 + startX, i * 5 + startY, 5, 5, color);
            }
        }
    }

    printRectangleCaller(UNUSED, 225, 60, 600, 105, WHITE);
    uint8_t i = 0;

    uint8_t *msg = "Welcome to PinguinOS\0";
    while (*msg)
    {
        drawBigChar(*msg++, 86 + i++, 1, WHITE, ORANGE);
    }
    uint8_t *msg2 = "Pedro    Federico\0";
    i = 0;
    while (*msg2)
    {
        drawBigChar(*msg2++, 10 + i++, 23, WHITE, ORANGE);
    }
    uint8_t *msg3 = "Filipo    Agustin\0";
    i = 0;
    while (*msg3)
    {
        drawBigChar(*msg3++, 38 + i++, 23, WHITE, ORANGE);
    }
    printRectangleCaller(UNUSED, 110, 5, 35, 35, WHITE);
    printRectangleCaller(UNUSED, SCREEN_WIDTH - 145, 5, 35, 35, WHITE);
    printRectangleCaller(UNUSED, 120, 18, 23, 20, BLACK);
    printRectangleCaller(UNUSED, SCREEN_WIDTH - 143, 18, 23, 20, BLACK);
}
