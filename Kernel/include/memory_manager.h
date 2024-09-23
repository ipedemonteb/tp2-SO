#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>

void start_mm();
void * my_malloc(uint64_t size);
void my_free(void * p);

#endif