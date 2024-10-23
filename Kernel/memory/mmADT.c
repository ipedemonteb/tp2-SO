#include "../include/mmADT.h"
#include <stdint.h>
#include <unistd.h> 

typedef struct blockData * blockDataPtr;

struct blockData {
    blockDataPtr next;
    blockDataPtr prev;
    uint64_t size;
    void* block;
};

struct mmCDT {
    blockDataPtr first;     //First block of free list
    uint64_t size;
    uint64_t avail;         //Available memory
};

mmADT init_mm(void * start, uint64_t size) {
    uint8_t aux = (sizeof(struct mmCDT) + sizeof(struct blockData));
    //@TODO: Ver si se deja
    if(size <= aux + 1) {
        return NULL;
    }

    mmADT mm = start;
    mm->size = size;
    mm->first = (blockDataPtr)((uint8_t*)start + sizeof(struct mmCDT));
    mm->avail = size - aux;

    mm->first->size = mm->avail;
    mm->first->block = (void*)((uint8_t*)start + aux);
    mm->first->next = NULL;
    mm->first->prev = NULL;

    return mm;
}


void * mm_alloc(mmADT mm, uint64_t size) {
    if(size == 0 || size + sizeof(struct blockData) > mm->avail) {
        return NULL;
    }

    blockDataPtr iter = mm->first;

    while(iter != NULL) {
        if(iter->size >= size /* + sizeof(struct blockData) */) {
            break;
        }
        iter = iter->next;
    }

    if(iter == NULL) {
        return NULL;
    }
    
    uint64_t remaining = iter->size - size /*- sizeof(struct blockData)*/;

    if(remaining > sizeof(struct blockData)) {
        blockDataPtr newFreeBlock = (blockDataPtr)((uint8_t *)iter->block + size);
        newFreeBlock->block = (void *)((uint8_t *)newFreeBlock + sizeof(struct blockData));
        newFreeBlock->size = remaining - sizeof(struct blockData);

        if (iter->prev == NULL) {
            newFreeBlock->next = iter->next;
            newFreeBlock->prev = iter->prev;
            mm->first = newFreeBlock;
        }
        else {
            //disconect iter
            iter->prev->next = iter->next;
            if (iter->next != NULL) {
                iter->next->prev = iter->prev;
            }

            blockDataPtr p = iter->prev;
            while (p != NULL && p->size > newFreeBlock->size) {
                p = p->prev;
            }
            if (p == NULL) {
                newFreeBlock->next = mm->first;
                newFreeBlock->prev = NULL;
                if (mm->first != NULL) {
                    mm->first->prev = newFreeBlock; 
                }
                mm->first = newFreeBlock;
            }
            else {
                newFreeBlock->next = p->next;
                newFreeBlock->prev = p;
                if (p->next != NULL) {
                    p->next->prev = newFreeBlock;
                }
                p->next = newFreeBlock;
            }
        }

        iter->size = size;
        mm->avail -= size + sizeof(struct blockData);
    }
    else {
        if(iter->prev != NULL) {
            iter->prev->next = iter->next;
        }
        else {
            mm->first = iter->next;
        }
        if(iter->next != NULL) {
            iter->next->prev = iter->prev;
        }
        mm->avail -= size + remaining;
    }
    return iter->block;
}

//@TODO: Ver que el bloque mas chico se guarde como first
void mm_free(mmADT mm, void * mem) {
    if(mm == NULL || mem == NULL) {
        return;
    }
    blockDataPtr blockFreed = (blockDataPtr)((uint8_t *)mem - sizeof(struct blockData));
    mm->avail += blockFreed->size /* + sizeof(struct blockData) */;
    blockDataPtr iter = mm->first;
    blockDataPtr prev = NULL;

    while(iter != NULL && iter->size < blockFreed->size) {
        prev = iter;
        iter = iter->next;
    }

    blockFreed->next = iter;
    blockFreed->prev = prev;

    if(prev != NULL) {
        prev->next = blockFreed;
    }
    else {
        mm->first = blockFreed;
    }
    if(iter != NULL) {
        iter->prev = blockFreed;
    }

    //@TODO: chequear si fusionamos bloques
    return;
}