#ifndef MM_ADT_H
#define MM_ADT_H

#include <stdint.h>

typedef struct mmCDT * mmADT;

mmADT init_mm(void * start, uint64_t size);
void * mm_alloc(mmADT mm, uint64_t size);
void mm_free(mmADT mm, void * mem);
uint64_t mm_avail(mmADT mm);
uint64_t mm_size(mmADT mm);

#endif