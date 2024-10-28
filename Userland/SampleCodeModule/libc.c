#include <stdint.h>
#include <stdarg.h>
#include "./include/libc.h"
#include "./include/syscaller.h"

#define MIN(a,b) a > b ? b:a

void readString(uint8_t * buffer, int maxLength);
void printString(uint8_t * str);
void printInt(int num);

int printf(const uint8_t * fmt, ...) {
    va_list args;
    uint32_t ret = 0;
    va_start(args, fmt);
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd':
                    printInt(va_arg(args, int));
                    break;
                case 'c':
                    putChar(va_arg(args, int));
                    break;
                case 's':
                    printString(va_arg(args, uint8_t *));
                    break;
            }
        }
        else {
            uint8_t aux = putChar(*fmt);
            if (!aux) {
                return -1;
            }
            ret += aux;
        }
        fmt++;
    }

    va_end(args);
    return ret;
}

int putChar(uint8_t c) {
    return writeCaller(UNUSED, 1, &c, 1);
}

int getChar() {
    uint8_t c = 0;
    while (!c) {
        readCaller(UNUSED, &c, 1);
    }
    return c;
}

void cleanBuffer() {
    uint8_t c;
    while (readCaller(UNUSED, &c, 1)) {
        ;
    }
}

void scanf(const uint8_t * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd': {
                    int *num = va_arg(args, int *);
                    uint8_t buffer[10];
                    readString(buffer, 10);
                    *num = atoi(buffer);
                    break;
                }
                case 's': {
                    uint8_t *str = va_arg(args, uint8_t *);
                    readString(str, 100);
                    break;
                }
            }
        }
        fmt++;
    }
    va_end(args);
}

void readString(uint8_t * buffer, int maxLength) {
    int i = 0;
    uint8_t c;
    while ((c = getChar()) != '\n' && i < maxLength - 1) {
        buffer[i++] = c;
        putChar(c);
    }
    buffer[i] = '\0';
}

void printString(uint8_t * str) {
    while (*str) {
        putChar(*str++);
    }
}

void printInt(int num) {
    if (num == 0) {
        putChar('0');
        return;
    }
    if (num < 0) {
        putChar('-');
        num = -num;
    }
    uint8_t buffer[10];
    int i = 0;
    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }
    while (i > 0) {
        putChar(buffer[--i]);
    }
}

int atoi(const uint8_t * str) {
    int res = 0;
    int sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    }
    while (*str) {
        res = res * 10 + (*str - '0');
        str++;
    }
    return res * sign;
}

void reverse(uint8_t * str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        uint8_t temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void itos(int num, uint8_t * buffer) {
    int i = 0;
    uint8_t isNegative = 0;
    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }
    if (num < 0) {
        isNegative = 1;
        num = -num;
    }
    while (num != 0) {
        int digit = num % 10;
        buffer[i++] = digit + '0';
        num /= 10;
    }
    if (isNegative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';
    reverse(buffer, i);
    return;
}

int strcmp(int8_t * s1, int8_t * s2) {
    if (s1 == NULL && s2 == NULL) {
        return 0;
    }
    int ret = 0;
    int i = 0;
    do {
        ret = s1[i] - s2[i]; 
    } while (s1[i] && s2[i++] && !ret);
    return ret;
}

uint32_t strcpy(char * dest, const char * source){
    uint32_t i = 0;
    while (source[i]) {
        dest[i] = source[i];
        i++;
    }
    dest[i] = 0;
    return i;
}

int64_t char_idx(char *s, char c) {
    int64_t i = 0;
    
    while (s[i]) {
        if (s[i] == c) {
            return i;
        }
        i++;
    }
    
    return -1;
}

static char * last;

char * strtok(char * string, const char * delim) {
    if (string == NULL) {
        string = last;
    }

    if (string == NULL) {
        return NULL;
    }

    while (*string && char_idx(delim, *string) != -1) {
        string++;
    }
    
    if (!*string) {
        last = NULL;
        return NULL;
    }

    char *token_start = string;
    while (*string && char_idx(delim, *string) == -1) {
        string++;
    }

    if (*string) {
        *string = 0;
        last = string + 1; 
    } else {
        last = NULL;
    }

    return token_start;
}