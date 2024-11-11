#include "../include/memory_manager.h"

mmADT mm;

void start_mm() {
    mm = init_mm((void *)MEM_START, MEM_SIZE);
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
    info->used = mm_size(mm) - mm_avail(mm);
}