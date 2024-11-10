#ifndef BUDDY_H_
#define BUDDY_H_

#include <stdint.h>
#include <stdio.h>
#include "videoDriver.h" // @todo: borrar, es para debuggeo

#define MEM_START 0x0000000000200000
#define MEM_END 0x0000000000400000
#define MEM_SIZE (MEM_END - MEM_START)

#define FREE     1
#define OCCUPIED 0

#define NULL ((void *) 0)

void init_buddy();
void * my_malloc(uint64_t size);
void my_free(void * p);


#endif