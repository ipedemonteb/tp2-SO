// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/memory_manager.h"

mmADT memory_man;

void start_mm() {
    memory_man = init_mm((void *)MEM_START, MEM_SIZE);
}

void * my_malloc(uint64_t size) {
    return mm_alloc(memory_man, size);
}

void my_free(void * p) {
    mm_free(memory_man, p);
}

void mem(mem_info * info) {
    info->total = mm_size(memory_man);
    info->free = mm_avail(memory_man);
    info->used = mm_size(memory_man) - mm_avail(memory_man);
}