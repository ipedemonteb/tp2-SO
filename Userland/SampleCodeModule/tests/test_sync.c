#include <stdint.h>
#include "../include/tests.h"
#include "../include/libc.h"
#define TOTAL_PAIR_PROCESSES    2
#define SEM_ID                  1

int64_t global; // shared memory

void slowInc(int64_t * p, int64_t inc) {
  int64_t aux = * p;
  yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

/*
argv[0] = n --> de a cuanto incremento/decremento
argv[1] = inc --> 1 o -1 para saber si hacgo dec o inc
argv[2] = use_sem --> 0 o 1 para saber si uso semaforos o no
*/

uint64_t my_process_inc(uint64_t argc, char * argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if(argc != 3) {
    return -1;
  }

  if((n = satoi(argv[0])) <= 0) {
    return -1;
  }

  if((inc = satoi(argv[1])) == 0) { 
    return -1;
  }
  if((use_sem = satoi(argv[2])) < 0) { 
    return -1;
  }

  /* if(use_sem) {
     int8_t sem_open_result = sem_open(SEM_ID, 1);
     if(sem_open_result == -1) {
       return -1;
     }
  } */

  uint64_t i;
  for(i = 0; i < n; i++) {
    if(use_sem) {
      sem_wait(SEM_ID);
    }
    slowInc(&global, inc);
    if(use_sem) {
      sem_post(SEM_ID);
    }
  }

   /* if(use_sem) {
     sem_close(SEM_ID);
  } */

  return 0;
}

void test_sync(uint8_t argc, char * argv[]) { // { n, use_sem, 0 }
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if(argc != 2) {
    printf("Incorrect argument count. Expected: 2");
    return;
  }

  char * argvDec[] = {argv[0], "-1", argv[1], 0};
  char * argvInc[] = {argv[0], "1", argv[1], 0};

  global = 0;

  int8_t sem_open_result = sem_open(SEM_ID, 1);
  if(sem_open_result == -1) {
    printf("Error while opening semaphore");
    return;
  }

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = create_process(my_process_inc, 3, argvDec, "my_process_inc", 0);
    pids[i + TOTAL_PAIR_PROCESSES] = create_process(my_process_inc, 3, argvInc, "my_process_inc", 0);
  }

  //wait_children();
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    wait_pid(pids[i], BLOCK);
    wait_pid(pids[i + TOTAL_PAIR_PROCESSES], BLOCK);
  }

  sem_close(SEM_ID);

  printf("test_sync: final global value %d\n", global);

  return;
}