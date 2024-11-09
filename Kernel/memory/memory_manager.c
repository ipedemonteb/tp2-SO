#include "../include/memory_manager.h"


#define START 0x0000000000200000
#define END 0x0000000000400000

mmADT mm;

void start_mm() {
    mm = init_mm((void *)START, END - START);
}

void * my_malloc(uint64_t size) {
    return mm_alloc(mm, size);
}

void my_free(void * p) {
    mm_free(mm, p);
}

void mem(mem_info * info) {
    info->total = mm_size(mm);
    info->free = mm_avail(mm);
    info-> used = mm_size(mm) - mm_avail(mm);
}