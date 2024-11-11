#include "../include/mmADT.h"
#include <stdint.h>
#include <unistd.h> 

typedef struct block_data * block_data_ptr;

struct block_data {
  block_data_ptr next;
  block_data_ptr prev;
  uint64_t size;
  void * block;
};

struct mmCDT {
  block_data_ptr first;     //First block of free list
  uint64_t size;
  uint64_t avail;         //Available memory
};

mmADT init_mm(void * start, uint64_t size) {
  uint8_t aux = (sizeof(struct mmCDT) + sizeof(struct block_data));
  if (size <= aux + 1) {
      return NULL;
  }

  mmADT mm = start;
  mm->size = size;
  mm->first = (block_data_ptr) ((uint8_t *) start + sizeof(struct mmCDT));
  mm->avail = size - aux;

  mm->first->size = mm->avail;
  mm->first->block = (void *) ((uint8_t *) start + aux);
  mm->first->next = NULL;
  mm->first->prev = NULL;

  return mm;
}


void * mm_alloc(mmADT mm, uint64_t size) {
  if(size == 0 || size + sizeof(struct block_data) > mm->avail) {
      return NULL;
  }

  block_data_ptr iter = mm->first;

  while(iter != NULL) {
      if(iter->size >= size) {
          break;
      }
      iter = iter->next;
  }

  if(iter == NULL) {
      return NULL;
  }
  
  uint64_t remaining = iter->size - size;

  if(remaining > sizeof(struct block_data)) {
    block_data_ptr new_free_block = (block_data_ptr) ((uint8_t *) iter->block + size);
    new_free_block->block = (void *)((uint8_t *) new_free_block + sizeof(struct block_data));
    new_free_block->size = remaining - sizeof(struct block_data);

    if (iter->prev == NULL) {
        new_free_block->next = iter->next;
        new_free_block->prev = iter->prev;
        mm->first = new_free_block;
    }
    else {
      //disconect iter
      iter->prev->next = iter->next;
      if (iter->next != NULL) {
        iter->next->prev = iter->prev;
      }

      block_data_ptr p = iter->prev;
      while (p != NULL && p->size > new_free_block->size) {
        p = p->prev;
      }
      if (p == NULL) {
        new_free_block->next = mm->first;
        new_free_block->prev = NULL;
        if (mm->first != NULL) {
            mm->first->prev = new_free_block; 
        }
        mm->first = new_free_block;
      }
      else {
        new_free_block->next = p->next;
        new_free_block->prev = p;
        if (p->next != NULL) {
            p->next->prev = new_free_block;
        }
        p->next = new_free_block;
      }
    }
    iter->size = size;
    mm->avail -= size + sizeof(struct block_data);
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

void mm_free(mmADT mm, void * mem) {
    if(mm == NULL || mem == NULL) {
        return;
    }
    block_data_ptr block_freed = (block_data_ptr)((uint8_t *) mem - sizeof(struct block_data));
    mm->avail += block_freed->size;
    block_data_ptr iter = mm->first;
    block_data_ptr prev = NULL;

    while(iter != NULL && iter->size < block_freed->size) {
        prev = iter;
        iter = iter->next;
    }

    block_freed->next = iter;
    block_freed->prev = prev;

    if(prev != NULL) {
        prev->next = block_freed;
    }
    else {
        mm->first = block_freed;
    }
    if(iter != NULL) {
        iter->prev = block_freed;
    }
    return;
}

uint64_t mm_avail(mmADT mm) {
    return mm->avail;
}

uint64_t mm_size(mmADT mm) {
    return mm->size;
}