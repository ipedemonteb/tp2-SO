#include <eliminator.h>
#include <stdint.h>
#include <syscaller.h>
#include <font.h>
#include <libc.h>
#include <sounds.h>
#include <stdarg.h>

typedef enum DIRECTION
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} DIRECTION;
typedef struct SNAKE
{
    uint64_t x;
    uint64_t y;
    DIRECTION direction;
    uint32_t color;
    uint8_t losses;
    uint8_t keys[4];
    uint16_t timeAlive;

} SNAKE;

#define PIXEL_SIZE 8
#define WIDTH 1024 / PIXEL_SIZE
#define HEIGHT 768 / PIXEL_SIZE
#define BLACK 0x00000000
#define RED 0x00FF0000
#define GREEN 0x0000FF00
#define PINK 0x00DD00FF
#define BLUE 0x000000FF
#define WHITE 0x00FFFFFF
#define GRAY 0x00303030

uint32_t gameSpeed;
uint8_t amountPlayers;
SNAKE snake1;
SNAKE snake2;

uint32_t gameState[WIDTH][HEIGHT];

uint64_t snakeInitPosition[2][2] = {{WIDTH / 2, HEIGHT / 5}, {WIDTH / 2, (HEIGHT * 4) / 5}};

void printBigPixel(uint16_t x, uint16_t y, uint32_t color)
{
    printRectangleCaller(UNUSED, x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE, color);
    return;
}

void gamePrintf(uint32_t x, uint32_t y, const uint8_t *fmt, ...)
{
    uint32_t x0 = x;
    uint32_t y0 = y;
    va_list args;
    va_start(args, fmt);
    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case 's':
            {
                char *toPrintStr;
                toPrintStr = va_arg(args, char *);
                while (*toPrintStr)
                {
                    drawBigChar(*toPrintStr, x0, y0, RED, BLACK);
                    x0++;
                    toPrintStr++;
                }
            }
            }
        }
        else if (*fmt == '\n')
        {
            y0++;
            x0 = x;
        }
        else if (*fmt == '\t')
            x0 += 4;
        else
            drawBigChar(*fmt, x0, y0, RED, BLACK);
        fmt++;
        x0 += 1;
    }
    va_end(args);
}

void gameMain()
{
    uint32_t exitGame = 0;
    while (!exitGame)
    {
        eliminatorMenu();
        uint32_t gameOver = 0;
        while (!gameOver)
        {
            startGame();
            initializeGameScene();
            uint8_t score1[4];
            uint8_t score2[4];
            if (amountPlayers == 2)
            {
                itos(snake2.losses, &score2);
                itos(snake1.losses, &score1);
                gamePrintf(790, 1, "Green %s - %s Purple", score2, score1);
            }
            else
            {
                itos((snake1.timeAlive) / 20, score1);
                gamePrintf(790, 1, "Your score was: %s", score1);
            }
            gamePrintf(720, 9, "\tGame Over, BOT!\n\nPress 'Space' to continue\n  'Esc' to go to menu\n 'E' to exit the game");
            int validKey = 0;
            while (!validKey)
            {
                uint8_t key = keyboardKeyCaller(UNUSED);
                if (key == SPACE)
                    validKey = 1;
                else if (key == ESCAPE)
                {
                    gameOver = 1;
                    validKey = 1;
                }
                else if (key == E_KEY)
                {
                    gameOver = 1;
                    exitGame = 1;
                    validKey = 1;
                }
            }
        }
    }
}

void eliminatorMenu()
{
    initializeGameScene();
    gameSpeed = 0;
    uint8_t flag = 1;
    gamePrintf(720, 9, "\tLa Vibora: El Juego\n\nIntroduzca la cantidad de jugadores\n\t\t[ 1 - 2 ]");
    uint8_t c;
    while (flag)
    {
        c = getChar();
        if (c >= '1' && c <= '2')
        {
            amountPlayers = c - '0';
            drawScene();
            flag = 0;
            eliminatorChoiceSound();
        }
    }
    snake1.losses = 0;
    if (amountPlayers == 2)
        snake2.losses = 0;
    flag = 1;
    gamePrintf(720, 9, "Ingrese la velocidad de la vibora:\n\n\t\t[ 1 - 4 ]");
    while (flag)
    {
        c = getChar();
        if (c >= '1' && c <= '4')
        {
            gameSpeed = 20 - (c - '0') * 2;
            drawScene();
            flag = 0;
            eliminatorChoiceSound();
        }
    }
    return;
}

void initializeGameScene()
{
    for (uint32_t i = 0; i < WIDTH; i++)
        for (uint32_t j = 0; j < HEIGHT; j++)
        {
            if (i == 0 || i == WIDTH - 1 || j == 0 || j == HEIGHT - 1)
                gameState[i][j] = RED;
            else
                gameState[i][j] = BLACK;
        }
    drawScene();
    snake1.x = WIDTH / 2;
    snake1.y = HEIGHT / 5;
    snake1.direction = DOWN;
    snake1.color = GREEN;
    snake1.keys[0] = UP_ARROW;
    snake1.keys[1] = DOWN_ARROW;
    snake1.keys[2] = LEFT_ARROW;
    snake1.keys[3] = RIGHT_ARROW;
    if (amountPlayers == 2)
    {
        snake2.x = WIDTH / 2;
        snake2.y = (HEIGHT * 4) / 5;
        snake2.direction = UP;
        snake2.color = PINK;
        snake2.keys[0] = 'w';
        snake2.keys[1] = 's';
        snake2.keys[2] = 'a';
        snake2.keys[3] = 'd';
    }
    return;
}

void drawScene()
{
    for (uint32_t i = 0; i < WIDTH; i++)
        for (uint32_t j = 0; j < HEIGHT; j++)
            printBigPixel(i, j, gameState[i][j]);
    return;
}

void startGame()
{
    initializeGameScene();
    uint8_t crash1 = 0;
    uint8_t crash2 = 0;
    snake1.timeAlive = 0;
    snake2.timeAlive = 0;
    cleanBuffer();

    /*printBigPixel(snake1.x, snake1.y, snake1.color);
    if (amountPlayers == 2)
        printBigPixel(snake2.x, snake2.y, snake2.color);
    startGameSound();*/
    while (!crash1 && !crash2)
    {
        snake1.timeAlive++;
        checkDir(&snake1);
        updatePos(&snake1);
        if (gameState[snake1.x][snake1.y] != BLACK)
            crash1 = 1;
        printBigPixel(snake1.x, snake1.y, snake1.color);
        gameState[snake1.x][snake1.y] = snake1.color;

        if (amountPlayers == 2)
        {
            snake2.timeAlive++;
            checkDir(&snake2);
            updatePos(&snake2);
            if (gameState[snake2.x][snake2.y] != BLACK)
                crash2 = 1;
            printBigPixel(snake2.x, snake2.y, snake2.color);
            gameState[snake2.x][snake2.y] = snake2.color;
        }
        waitCaller(UNUSED, gameSpeed);
    }
    endGameSound();
    if (!(crash1 && crash2))
    {
        if (crash2)
            snake2.losses++;
        else if (crash1)
            snake1.losses++;
    }
    endGameSound();
    return;
}

void updatePos(SNAKE *snake)
{
    switch (snake->direction)
    {
    case UP:
    {
        snake->y--;
        break;
    }
    case DOWN:
    {
        snake->y++;
        break;
    }
    case LEFT:
    {
        snake->x--;
        break;
    }
    case RIGHT:
    {
        snake->x++;
        break;
    }
    }
    return;
}

void checkDir()
{
    uint8_t key = 0;
    readCaller(UNUSED, &key, 1);
    if (key)
    {
        if (key == snake1.keys[0] && snake1.direction != DOWN)
            snake1.direction = UP;
        else if (key == snake1.keys[1] && snake1.direction != UP)
            snake1.direction = DOWN;
        else if (key == snake1.keys[2] && snake1.direction != RIGHT)
            snake1.direction = LEFT;
        else if (key == snake1.keys[3] && snake1.direction != LEFT)
            snake1.direction = RIGHT;

        if (amountPlayers == 2)
        {
            if (key == snake2.keys[0] && snake2.direction != DOWN)
                snake2.direction = UP;
            else if (key == snake2.keys[1] && snake2.direction != UP)
                snake2.direction = DOWN;
            else if (key == snake2.keys[2] && snake2.direction != RIGHT)
                snake2.direction = LEFT;
            else if (key == snake2.keys[3] && snake2.direction != LEFT)
                snake2.direction = RIGHT;
        }
    }
    return;
}
