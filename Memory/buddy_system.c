#include "mmADT.h"

#define SMALLEST_BLOCK 1
#define NULL (void*) 0
struct free_block{
    uint64_t size;
    void * start;
};

struct mmCDT{
    uint64_t size;
    uint8_t block_count; 
    uint8_t current_free_block_count;
    struct free_block * free_blocks;
};

uint8_t max_blocks(uint64_t size , struct free_block * free_blocks){
    uint8_t count = 0;
    size /= 2;
    free_blocks[count++].size = SMALLEST_BLOCK;
    while (size < SMALLEST_BLOCK) {
        size /= 2;
        free_blocks[count].size = free_blocks[count-1].size * 2;
        count++;
    }
    return count;
}


mmADT init_mm(void * start, uint64_t size){
    if (size < sizeof(struct mmCDT) + sizeof(struct free_block) + SMALLEST_BLOCK) {
        return NULL;
    }
    mmADT mm = start;
    mm->free_blocks = start + sizeof(struct mmCDT);
    mm->block_count = max_blocks(size - sizeof(struct mmCDT) , mm->free_blocks);
    //@TODO: ver que onda el espacio que se usa para el vector de punteros si la memoria queda medio justita
    mm->free_blocks[mm->block_count-1].start =  &mm->free_blocks[mm->block_count];
    mm->current_free_block_count = 1;
    return mm; 
}

void * alloc(mmADT mm ,uint64_t size){
    uint8_t current = mm->block_count - 1;
    
}

void free(mmADT mm){

}