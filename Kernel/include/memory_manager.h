#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "../include/mmADT.h"
#include <stdint.h>

typedef struct mem_info {
  uint64_t total;
  uint64_t used;
  uint64_t free;
} mem_info;

#define MEM_START 0x0000000000200000
#define MEM_END 0x0000000000400000
#define MEM_SIZE (MEM_END - MEM_START)

#define FREE 1
#define OCCUPIED 0

#define NULL (void *)0

void start_mm();
void *my_malloc(uint64_t size);
void my_free(void *p);
void mem(mem_info *info);

#endif