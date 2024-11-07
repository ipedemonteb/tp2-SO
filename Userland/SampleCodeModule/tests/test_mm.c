#include "../include/tests.h"
#include "../include/libc.h"

#define MAX_BLOCKS 128

typedef struct MM_rq {
  void * address;
  uint32_t size;
} mm_rq;

uint64_t test_mm1(uint64_t argc, uint64_t max_memory) {
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;

  if (argc != 1) {
    return -1;
  }

  
  while (1) {
    rq = 0;
    total = 0;
    // Request as many blocks as we can
    while (total < max_memory) {
      mm_rqs[rq].size = rq + 1;
      mm_rqs[rq].address = my_malloc(mm_rqs[rq].size);
      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++) {
      if (mm_rqs[i].address) {
        my_memset(mm_rqs[i].address, i, mm_rqs[i].size);
      }
    }

    // Check 
    for (i = 0; i < rq; i++) {
      if (mm_rqs[i].address) {
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          s_draw_line("test_mm: check failed",0,1);
          s_off_cursor();
          return -1;
        }
      }
    }

    // Free
    for (i = 0; i < rq; i++) {
      if (mm_rqs[i].address) {
        my_free(mm_rqs[i].address);
      }
    }
  }
}

void test_mm(uint8_t argc, char * argv[]) {
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1) {
    printf("Incorrect argument count. Expected: 1\n");
    return;
  }

  if ((max_memory = satoi(argv[0])) <= 0) { //@todo: esta raro el tema de los negativos
    printf("Max memory must be greater than 0\n");
    return;
  }

  while (1) {
    rq = 0;
    total = 0;
    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = my_malloc(mm_rqs[rq].size);
      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++) {
      if (mm_rqs[i].address) {
        my_memset(mm_rqs[i].address, i, mm_rqs[i].size);
      }
    }

    // Check
    for (i = 0; i < rq; i++) {
      if (mm_rqs[i].address) {
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size )) {
          printf("Memory Error.");
          return;
        }
      }
    }

    // Free
    for (i = 0; i < rq; i++) {
      if (mm_rqs[i].address) {
        my_free(mm_rqs[i].address);
      }
    }
  }

}