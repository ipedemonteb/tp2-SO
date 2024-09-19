#include <libc.h>
#include <stdint.h>
#include <stdarg.h>
#include <syscaller.h>

void readString(char *buffer, int maxLength);
void printString(char *str);
void printInt(int num);

int printf(const char *fmt, ...)
{
    va_list args;
    uint32_t ret = 0;
    va_start(args, fmt);
    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case 'd':
                printInt(va_arg(args, int));
                break;
            case 'c':
                putChar(va_arg(args, int));
                break;
            case 's':
                printString(va_arg(args, char *));
                break;
            }
        }
        else
        {
            uint8_t aux = putChar(*fmt);
            if (!aux)
                return -1;
            ret += aux;
        }
        fmt++;
    }

    va_end(args);
    return ret;
}

int putChar(char c)
{
    return writeCaller(UNUSED, 1, &c, 1);
}

int getChar()
{
    char c = 0;
    while (!c)
        readCaller(UNUSED, &c, 1);
    return c;
}

void cleanBuffer()
{
    char c;
    while (readCaller(UNUSED, &c, 1))
        ;
}

void scanf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case 'd':
            {
                int *num = va_arg(args, int *);
                char buffer[10];
                readString(buffer, 10);
                *num = atoi(buffer);
                break;
            }
            case 's':
            {
                char *str = va_arg(args, char *);
                readString(str, 100);
                break;
            }
            }
        }
        fmt++;
    }
    va_end(args);
}
void readString(char *buffer, int maxLength)
{
    int i = 0;
    char c;
    while ((c = getChar()) != '\n' && i < maxLength - 1)
    {
        buffer[i++] = c;
        putChar(c);
    }
    buffer[i] = '\0';
}
void printString(char *str)
{
    while (*str)
        putChar(*str++);
}
void printInt(int num)
{
    if (num == 0)
    {
        putChar('0');
        return;
    }
    if (num < 0)
    {
        putChar('-');
        num = -num;
    }
    char buffer[10];
    int i = 0;
    while (num > 0)
    {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }
    while (i > 0)
        putChar(buffer[--i]);
}
int atoi(const char *str)
{
    int res = 0;
    int sign = 1;

    if (*str == '-')
    {
        sign = -1;
        str++;
    }

    while (*str)
    {
        res = res * 10 + (*str - '0');
        str++;
    }

    return res * sign;
}

void reverse(char *str, int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end)
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void itos(int num, char *buffer)
{
    int i = 0;
    char isNegative = 0;

    if (num == 0)
    {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }

    if (num < 0)
    {
        isNegative = 1;
        num = -num;
    }

    while (num != 0)
    {
        int digit = num % 10;
        buffer[i++] = digit + '0';
        num /= 10;
    }

    if (isNegative)
    {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    reverse(buffer, i);

    return;
}

int strcmp(char *s1, char *s2)
{
    if (s1 == NULL && s2 == NULL)
    {
        return 0;
    }
    int ret = 0;
    int i = 0;
    do
    {
        ret = s1[i] - s2[i];
    } while (s1[i] && s2[i++] && !ret);
    return ret;
}
