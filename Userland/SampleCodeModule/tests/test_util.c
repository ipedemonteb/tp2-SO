#include <stdint.h>
#include "../include/syscall.h"
#include "../include/shell_graphics.h"
#include "../include/libc.h"

// Random
static uint32_t m_z = 362436069;
static uint32_t m_w = 521288629;

uint32_t GetUint() {
  m_z = 36969 * (m_z & 65535) + (m_z >> 16);
  m_w = 18000 * (m_w & 65535) + (m_w >> 16);
  return (m_z << 16) + m_w;
}

uint32_t GetUniform(uint32_t max) {
  uint32_t u = GetUint();
  return (u + 1.0) * 2.328306435454494e-10 * max;
}

// Memory
uint8_t memcheck(void *start, uint8_t value, uint32_t size) {
  uint8_t *p = (uint8_t *)start;
  uint32_t i;

  for (i = 0; i < size; i++, p++) {
    if (*p != value) {
      return 0;
    }
  }

  return 1;
}

void * my_memset(void * destination, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t)c;
	char * dst = (char *)destination;
	while(length--) {
		dst[length] = chr;
	}
	return destination;
}

// Parameters
int64_t satoi(uint8_t * str) {
  uint64_t i = 0;
  int64_t res = 0;
  int8_t sign = 1;

  if (!str) {
    return 0;
  }
  if (str[i] == '-') {
    i++;
    sign = -1;
  }
  for (; str[i] != '\0'; ++i) {
    if (str[i] < '0' || str[i] > '9')
      return 0;
    res = res * 10 + str[i] - '0';
  }

  return res * sign;
}

// Dummies
void bussy_wait(uint64_t n) {
  uint64_t i;
  for (i = 0; i < n; i++)
    ;
}

void endless_loop() {
  int i = 0 , j = 10;
  while (1){
    if (i == 3)
    {
      j += 1;
    }
    i++;
  }
}

void numToStr(int num, int8_t * str) {
    int i = 0, j;
    int8_t temp;
    do {
        str[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);
    str[i] = '\0';
    for (j = 0; j < i / 2; ++j) {
        temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

void endless_loop_print(uint64_t wait) {
  int64_t pid = get_current_pid();
  int8_t buffer[20];
  numToStr(pid, buffer);
  while (1) {
    /* int8_t pid_s[8];
    itos(pid, pid_s);
    s_draw_line(pid_s,0,1);
    s_off_cursor(); */
    bussy_wait(wait);
  }
}