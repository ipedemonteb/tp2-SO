// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

  mmADT memory_man = start;
  memory_man->size = size;
  memory_man->first = (block_data_ptr) ((uint8_t *) start + sizeof(struct mmCDT));
  memory_man->avail = size - aux;

  memory_man->first->size = memory_man->avail;
  memory_man->first->block = (void *) ((uint8_t *) start + aux);
  memory_man->first->next = NULL;
  memory_man->first->prev = NULL;

  return memory_man;
}


void * mm_alloc(mmADT memory_man, uint64_t size) {
  if(size == 0 || size + sizeof(struct block_data) > memory_man->avail) {
      return NULL;
  }

  block_data_ptr iter = memory_man->first;

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
        iter->next->prev = new_free_block;
        new_free_block->prev = iter->prev;
        memory_man->first = new_free_block;
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
        new_free_block->next = memory_man->first;
        new_free_block->prev = NULL;
        if (memory_man->first != NULL) {
            memory_man->first->prev = new_free_block; 
        }
        memory_man->first = new_free_block;
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
    memory_man->avail -= size + sizeof(struct block_data);
  }
  else {
    if(iter->prev != NULL) {
      iter->prev->next = iter->next;
    }
    else {
      memory_man->first = iter->next;
    }
    if(iter->next != NULL) {
      iter->next->prev = iter->prev;
    }
    memory_man->avail -= size + remaining;
  }
  return iter->block;
}

void mm_free(mmADT memory_man, void * mem) {
    if(memory_man == NULL || mem == NULL) {
        return;
    }
    block_data_ptr block_freed = (block_data_ptr)((uint8_t *) mem - sizeof(struct block_data));
    memory_man->avail += block_freed->size;
    block_data_ptr iter = memory_man->first;
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
        memory_man->first = block_freed;
    }
    if(iter != NULL) {
        iter->prev = block_freed;
    }
    return;
}

uint64_t mm_avail(mmADT memory_man) {
    return memory_man->avail;
}

uint64_t mm_size(mmADT memory_man) {
    return memory_man->size;
}