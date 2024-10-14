#include "../include/mmADT.h"

#define START 0x50000
#define END 0x9FFFF

mmADT mm;
char mem[END-START];

void start_mm() {
    mm = init_mm((void *)START, END - START);
}

void * my_malloc(uint64_t size) {
    return mm_alloc(mm, size);
}

void my_free(void * p) {
    mm_free(mm, p);
}