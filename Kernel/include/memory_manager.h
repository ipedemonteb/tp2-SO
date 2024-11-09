#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include "../include/mmADT.h"

typedef struct mem_info {
	uint64_t total;
	uint64_t used;
	uint64_t free;
} mem_info;

void start_mm();
void * my_malloc(uint64_t size);
void my_free(void * p);
void mem(mem_info * info);

#endif