#include "test_util.h"
#include "test_mm.h"
#include <stdio.h>
#include "memory_manager.h"
#include <string.h>

#define MAX_BLOCKS 128

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm1(uint64_t argc, char *argv[]) {
  start_mm();
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1)
    return -1;

  if ((max_memory = satoi(argv[0])) <= 0)
    return -1;
  while (1) {
    rq = 0;
    total = 0;

    // Request as many blocks as we can
    while (total < max_memory) {
      mm_rqs[rq].size = rq + 1;
      mm_rqs[rq].address = my_malloc(mm_rqs[rq].size);
      if (mm_rqs[rq].address) {
        printf("puntero: %p , tamaño:%d , maximo:%lu \n",mm_rqs[rq].address,mm_rqs[rq].size , max_memory);
        total += mm_rqs[rq].size;
        rq++;
      }

    }
    printf("total:%d , maximo:%lu\n", total,max_memory);
    puts("hola");

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        memset(mm_rqs[i].address, i, mm_rqs[i].size);

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          printf("test_mm ERROR\n");
          return -1;
        }

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        my_free(mm_rqs[i].address);
  }
}

uint64_t test_mm(uint64_t argc, char *argv[]) {
  start_mm();
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1)
    return -1;

  if ((max_memory = satoi(argv[0])) <= 0)
    return -1;

  while (1) {
    rq = 0;
    total = 0;

    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = my_malloc(mm_rqs[rq].size);
      if (mm_rqs[rq].address) {
        printf("puntero: %p , tamaño:%d\n",mm_rqs[rq].address,mm_rqs[rq].size );
        total += mm_rqs[rq].size;
        rq++;
      }
    }
    printf("total:%d\n", total);
    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        memset(mm_rqs[i].address, i, mm_rqs[i].size);

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size )) {
          printf("test_mm ERROR\n");
          return -1;
        }

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        my_free(mm_rqs[i].address);
  }
}
