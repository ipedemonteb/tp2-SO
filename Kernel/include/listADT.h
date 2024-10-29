#ifndef _LIST_H
#define _LIST_H

#include <stdint.h>
#include "memory_manager.h"

#define NULL (void *) 0

typedef int (*compare)(void * e1, void * e2);
typedef struct listCDT * listADT;

void add(listADT list, void * elem);
listADT new_list(compare cmp);
void free_list(listADT list);
void to_begin(listADT list);
uint8_t has_next(const listADT list);
void * next(listADT list);
void * peek(listADT list);
void * poll(listADT list);
void queue(listADT list , void * elem);
#endif