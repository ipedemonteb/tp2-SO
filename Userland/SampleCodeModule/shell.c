#include <stdint.h>
#include "./include/shell.h"
#include "./include/libc.h"
#include "./include/userLib.h"
#include "./include/sounds.h"
#include "./include/eliminator.h"
#include "./include/syscall.h"
#include "./include/test_process.h"

#define BUFF_MAX 4096
#define WHITE 0x00FFFFFF
#define BLACK 0
#define DELETE 127
#define LEFT_ARROW 251
#define RIGHT_ARROW 252
#define UP_ARROW 253
#define DOWN_ARROW 254
#define LETTERS 'z' - 'a' + 1
#define WORDS 4

typedef struct command {
    int8_t * name;
    void (*function)();
} command;

static void clearCmd();
static void div0();
static void exit();
static void eliminator();
static void fontSmall();
static void fontBig();
static void getTime();
static void help();
static void invalidOpCode();
static void prio();
static void processes();
static void mm();

static command commands[LETTERS][WORDS] = {
    {{0,0}},  //a
    {{0,0}},  //b
    {{(int8_t *)"clear", clearCmd}, {0, 0}}, 
    {{(int8_t *)"div0", div0}, {0, 0}}, 
    {{(int8_t *)"eliminator", eliminator}, {(int8_t *)"exit", exit}}, 
    {{(int8_t *)"fontBig", fontBig}, {(int8_t *)"fontSmall", fontSmall}}, 
    {{(int8_t *)"getTime", getTime}, {0, 0}},
    {{(int8_t *)"help", help}, {0, 0}}, 
    {{(int8_t *)"invalidOpCode", invalidOpCode}, {0, 0}},
    {{0,0}},  //j
    {{0,0}},  //k
    {{0,0}},  //l
    {{0,0}},  //m
    {{0,0}},  //n
    {{0,0}},  //o
    {{0,0}},  //p
    {{0,0}},  //q
    {{0,0}},  //r
    {{0,0}},  //s
    {{"test_mm",mm},{"test_prio",prio},{"test_processes",processes},{0,0}}   //t
};

static int8_t * commandNotFoundMsg = (int8_t *)"Command not found. Type help for a list of commands";
static uint8_t cNotFoundSize = 51;
static int8_t * helpMsg = (int8_t *)"List of commands: clear, div0, eliminator, exit, fontBig, fontSmall, getTime, help, invalidOpCode";
static uint8_t hMsgSize = 97;
static int8_t * waitMsg = (int8_t *)"Press any key to continue";

static uint16_t currentY;
static uint16_t currentX;
static uint16_t width;
static uint16_t height;
static int8_t buffer[4096];
static uint16_t count;
static uint16_t offsets[4096] = {0};
static uint16_t lineCount;
static uint16_t firstLineOnScreen;
static uint16_t leftSteps;
static uint16_t currentLine;
static uint16_t previousCount;
static uint8_t exitFlag;
static uint8_t fontSize;
static uint8_t reset;

void sPrintChar(uint8_t c) {
    printChar(c, currentX, currentY, WHITE, BLACK);
}

void sPrintSelected(uint8_t c) {
    printChar(c, currentX, currentY, BLACK, WHITE);
}

void startNewLine() {
    currentX = 0;
    sPrintChar('$');
    currentX++;
    sPrintChar('>');
    currentX++;
}

void clearLineFrom(uint16_t x, uint16_t to) {
    uint16_t auxX = currentX;
    currentX = x;
    while (currentX < width) {
        printRectangle(currentX * (8 * fontSize), currentY * (16 * fontSize), 8 * fontSize, 16 * fontSize, BLACK);
        currentX++;
    }
    currentX = auxX;
}

void clear() {
    uint16_t auxX = currentX, auxY = currentY;
    currentX = 0;
    currentY = 0;
    while (currentY < height) {
        clearLineFrom(0, width);
        currentY++;
    }
    currentX = auxX;
    currentY = auxY;
}

void sMoveScreenUp(uint8_t n) {
    clear();
    uint16_t y = currentY - n + 1, l = firstLineOnScreen + n;
    currentY = 0;

    while (y != currentY) {
        startNewLine();
        for (uint16_t i = 0; i < offsets[l + 1] - offsets[l]; i++) {
            if (currentX == width) {
                currentY++;
                currentX = 0;
            }
            sPrintChar(buffer[offsets[l] + i]);
            currentX++;
        }
        l++;
        currentY++;
    }
    firstLineOnScreen += n;
}

void sPrintNewLine() {
    if (!reset) {
        if (currentY < height - 1) {
            currentY++;
        }
        else {
            sMoveScreenUp(1);
        }
    }
    startNewLine();
    reset = 0;
}

void printMsgAndWait(const int8_t * msg, uint8_t size) {
    if (currentY < height - 2) {
        currentY++;
    }
    else {
        sMoveScreenUp(2 + size / width);
    }
    currentX = 0;
    for (uint16_t i = 0; i < size; i++) {
        sPrintChar(msg[i]);
        if (currentX == width - 1) {
            currentX = 0;
            currentY++;
        }
        else {
            currentX++;
        }
    }
    currentY++;
    currentX = 0;
    for (uint8_t j = 0; waitMsg[j]; j++) {
        sPrintChar(waitMsg[j]);
        currentX++;
    }
    uint8_t c = 0;
    while (!(c = getChar())) {
        ;
    }
    for (uint8_t i = 0; i < 2 + size / width; i++) {
        clearLineFrom(0, i);
        currentY--;
    }
}

int8_t getCommandIdx(uint8_t c) {
    if (c <= 'Z') {
        return c - 'A';
    }
    if (c <= 'z')
    {
        return c - 'a';
    }
    return -1;
}

void sCheckCommand() {
    if (offsets[lineCount] == offsets[lineCount - 1]) {
        return;
    }
    uint8_t aux = buffer[offsets[lineCount]];
    uint8_t c = getCommandIdx(buffer[offsets[lineCount - 1]]);
    if (c < 0) {
        printMsgAndWait(commandNotFoundMsg, cNotFoundSize);
        return;
    }

    buffer[offsets[lineCount]] = 0;
    command * auxC = commands[c];
    for (int i = 0; i < WORDS; i++) {
        if (auxC[i].name != NULL) {
            int cmp = strcmp(buffer + offsets[lineCount - 1], auxC[i].name);
            if (cmp < 0) {
                break;
            }
            else if (cmp == 0) {
                auxC[i].function();
                buffer[offsets[lineCount]] = aux;
                return;
            }
        }
        else {
            break;
        }
    }
    printMsgAndWait(commandNotFoundMsg, cNotFoundSize);
    buffer[offsets[lineCount]] = aux;
}

void sDeleteChar() {
    if (offsets[lineCount - 1] == count - leftSteps) {
        return;
    }
    if (currentX == 0) {
        currentY--;
        currentX = width - 1;
    }
    else {
        currentX--;
    }
    uint16_t auxX = currentX, auxY = currentY;
    for (uint16_t i = 0; i <= leftSteps; i++) {
        buffer[count - leftSteps + i - 1] = buffer[count - leftSteps + i];
        sPrintChar(buffer[count - leftSteps + i]);
        if (currentX == width - 1) {
            currentX = 0;
            currentY++;
        }
        else {
            currentX++;
        }
    }
    sPrintChar(buffer[count]);
    currentX = auxX;
    currentY = auxY;
    sPrintSelected(buffer[count - leftSteps - 1]);
    count--;

    offsets[lineCount]--;
}

void sMoveLeft() {
    if (offsets[lineCount - 1] == count - leftSteps) {
        shellErrSound();
        return;
    }
    sPrintChar(buffer[count - leftSteps]);
    if (currentX == 0) {
        currentX = width - 1;
        currentY--;
    }
    else {
        currentX--;
    }
    leftSteps++;
    sPrintSelected(buffer[count - leftSteps]);
}

void sMoveRight() {
    if (leftSteps == 0) {
        shellErrSound();
        return;
    }
    sPrintChar(buffer[count - leftSteps]);
    if (currentX == width) {
        currentX = 0;
        currentY++;
    }
    else {
        currentX++;
    }
    leftSteps--;
    sPrintSelected(buffer[count - leftSteps]);
}

void sGetLastLine() {
    uint16_t currentOffset = offsets[lineCount] - offsets[lineCount - 1], l;
    do {
        if (currentLine) {
            currentLine--;
        }
        else {
            return;
        }
        l = offsets[currentLine + 1] - offsets[currentLine];
    } while (!l);
    previousCount += l;
    clearLineFrom(2, width);
    count -= currentOffset;
    currentX = 2;
    for (uint16_t i = 0; i < l; i++) {
        if (currentX == width) {
            if (currentY < height - 1) {
                currentY++;
            }
            else {
                sMoveScreenUp(1);
            }
            currentX = 0;
        }
        uint8_t aux = buffer[count - previousCount];
        buffer[count++] = aux;
        sPrintChar(aux);
        currentX++;
    }
    buffer[count] = ' ';
    offsets[lineCount] = count;
    sPrintSelected(' ');
}

void printBufferFrom(uint16_t start, uint16_t end) {
    for (int i = start; i < end; i++) {
        sPrintChar(buffer[i]);
        currentX++;
    }
}

void launchShell() {
    count = 0;
    lineCount = 1;
    firstLineOnScreen = 0;
    leftSteps = 0;
    currentLine = 1;
    previousCount = 0;
    fontSize = 1;
    reset = 0;
    width = getScreenWidth() / 8 / fontSize;
    height = getScreenHeight() / 16 / fontSize;
    clear();
    currentX = 0;
    currentY = 0;
    exitFlag = 0;
    startNewLine();
    sPrintSelected(' ');
    buffer[count] = ' ';

    uint8_t key;
    while (!exitFlag) {
        key = getChar();
        switch (key) {
            case RIGHT_ARROW:
                sMoveRight();
                break;
            case LEFT_ARROW:
                sMoveLeft();
                break;
            case UP_ARROW:
                sGetLastLine();
                break;
            case DOWN_ARROW:
                shellErrSound();
                break;
            case DELETE:
                sDeleteChar();
                break;
            case '\n':
                sPrintChar(buffer[count - leftSteps]);
                sCheckCommand();
                leftSteps = 0;
                previousCount = 0;
                sPrintNewLine();
                sPrintSelected(buffer[count]);
                offsets[lineCount++] = count;
                offsets[lineCount] = offsets[lineCount - 1]; // la linea esta vacia
                currentLine = lineCount;
                break;
            case '\t':
                if (offsets[lineCount] - offsets[lineCount - 1] == 1) {
                    uint8_t c = getCommandIdx(buffer[count - 1]);
                    if (c < 0) {
                        break;
                    }
                    
                    int8_t * aux = commands[c][0].name + 1;
                    while (*aux) {
                        sPrintChar(*aux);
                        buffer[count++] = *aux;
                        aux++;
                        currentX++;
                    }
                    offsets[lineCount] = count;
                    buffer[count] = ' ';
                    sPrintSelected(' ');
                }
            case 0:
                break;
            default: {
                uint16_t auxX = currentX, auxY = currentY;
                if (currentX + leftSteps >= width) {
                    currentY++;
                }
                currentX = (currentX + leftSteps) % width;
                for (uint16_t i = 0; i < leftSteps; i++) {
                    buffer[count - i] = buffer[count - i - 1];
                    sPrintChar(buffer[count - i]);
                    if (currentX == 0) {
                        currentX = width - 1;
                        currentY--;
                    }
                    else {
                        currentX--;
                    }
                }
                currentX = auxX;
                currentY = auxY;
                buffer[count - leftSteps] = key;
                count++;
                offsets[lineCount] = count;
                buffer[count] = ' ';
                sPrintChar(key);
                currentX++;
                if (currentX == width) {
                    if (currentY < height - 1) {
                        currentY++;
                    }
                    else {
                        sMoveScreenUp(1);
                    }
                    currentX = 0;
                }
                sPrintSelected(buffer[count - leftSteps]);
            }
        }
    }
    while (fontSizeDown()) {
        fontSize--;
    }
}

// comandos de la terminal

void fontBig() {
    if (fontSize == 2) {
        shellErrSound();
        return;
    }
    fontSizeUp();
    fontSize = 2;
    height /= 2;
    width /= 2;
    sMoveScreenUp((currentLine - firstLineOnScreen) / 2);
    reset = 1;
}

void exit() {
    exitFlag = 1;
    firstLineOnScreen = lineCount - 1;
    currentX = 0;
    currentY = 0;
}

void div0() {
    fontSize = 1;
    divZero();
}

void fontSmall() {
    if (fontSize == 1) {
        shellErrSound();
        return;
    }
    fontSizeDown();
    fontSize = 1;
    height *= 2;
    width *= 2;
    sMoveScreenUp(0);
    reset = 1;
}

void eliminator() {
    /* gameMain();
    cleanBuffer();
    sMoveScreenUp(0);
    reset = 1; */
}

void clearCmd() {
    clear();
    reset = 1;
    firstLineOnScreen = lineCount - 1;
    currentX = 0;
    currentY = 0;
}

void getTime() {
    int8_t clock[20];
    time(clock);
    printMsgAndWait(clock, 8);
}

void help() {
    printMsgAndWait(helpMsg, hMsgSize);
}

void invalidOpCode() {
    fontSize = 1;
    opcode();
}

void prio(){
    create_process(test_prio,0,0,"test_prio");
    wait_children();
}

void processes(){

}

void mm(){

}