#ifndef _LIST_H
#define _LIST_H

#include <stdint.h>
#include "memory_manager.h"

#ifndef NULL
#define NULL (void *) 0
#endif

typedef int (*compare)(void * e1, void * e2);
typedef struct listCDT * listADT;

listADT new_list(compare cmp);
void free_list(listADT list);
void to_begin(listADT list);
uint8_t has_next(const listADT list);
void * next(listADT list);
void * peek(listADT list);
void * poll(listADT list);
void queue(listADT list , void * elem);
uint8_t is_empty(listADT list);
void remove(listADT list, void * elem);

#endif