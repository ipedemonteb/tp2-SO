#include <stdint.h>
#include "../include/syscall.h"
#include "../include/videoDriver.h"
#include "../include/lib.h"
#include "../include/process.h" // Assuming process_info is defined here
#include "../include/process_manager.h"
#include "../include/memory_manager.h"   // Assuming my_malloc is declared here
#include "../include/syscalls.h"

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
      drawchar('0', j , j , WHITE , BLACK);
      j += 1;
    }
    i++;
  }
}

void numToStr(int num, int8_t *str) {
    int i = 0, j;
    int8_t temp;
    int isNegative = 0;
    if (num < 0) {
        isNegative = 1;
        num = -num;
    }
    do {
        str[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);
    if (isNegative) {
        str[i++] = '-';
    }
    str[i] = '\0';
    for (j = 0; j < i / 2; ++j) {
        temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

void endless_loop_print(uint64_t wait) {
  int64_t pid = my_getpid();
  int8_t buffer[20];
  numToStr(pid, buffer);
  while (1) {
    drawString(buffer, 25 , pid , WHITE , BLACK);
    bussy_wait(wait);
  }
}

void print_process_info() {
  printRectangle(0,20*16,getScreenWidth(),getScreenHeight(),BLACK);
  process_info p_info[sizeof(process_info) * QUANT];
  for(int i = 0; i < QUANT; i++) {
    p_info[i].name = my_malloc(20);
  }

  uint32_t x = 0;
  uint32_t y = 20;
  drawString((int8_t *)"PID    NAME    PRIORITY    STATUS    STCK B    STCK PTR", x, y, WHITE, BLACK);
  y += 1;

  uint8_t s = ps(p_info);
  int8_t id[20];
  int8_t * prio[20];
  int8_t stat[10];
  int8_t stack_base[10];
  int8_t stack_ptr[10];
  
  for(uint16_t i = 0; i < s; i++) {
    //id
    numToStr(p_info[i].pid, id);
    drawString(id, x, y, WHITE, BLACK);
    //name
    drawString((int8_t *)p_info[i].name, x + 5, y, WHITE, BLACK);
    //priority
    numToStr(p_info[i].priority, prio);
    drawString(prio, x + 20, y, WHITE, BLACK);
    //status
    numToStr(p_info[i].status, stat);
    drawString(stat, x + 30, y, WHITE, BLACK);
    //stack base
    uint64ToHexStr((uint64_t)p_info[i].stack_base, stack_base);
    drawString((int8_t *)stack_base, x + 40, y, WHITE, BLACK);
    //stack ptr
    uint64ToHexStr((uint64_t)p_info[i].stack_ptr, stack_ptr);
    drawString((int8_t *)stack_ptr, x + 50, y, WHITE, BLACK);
    y += 1;
  }
  for(int i = 0; i < QUANT; i++) {
    my_free(p_info[i].name);
  }
}