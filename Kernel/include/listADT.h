#ifndef _LIST_H
#define _LIST_H

#include <stdint.h>
#include "memory_manager.h"

#define NULL (void *) 0

typedef int (*compare)(void * e1, void * e2);
typedef struct listCDT * listADT;

void add(listADT list, void * elem);
listADT newList(compare cmp);
void freeList(listADT list);
void toBegin(listADT list);
uint8_t hasNext(const listADT list);
void * next(listADT list);
void * poll(listADT list);
void * queue(listADT list , void * elem);
#endif