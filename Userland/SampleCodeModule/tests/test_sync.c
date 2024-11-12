// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/libc.h"
#include "../include/tests.h"
#include <stdint.h>
#define TOTAL_PAIR_PROCESSES 2
#define SEM_ID 1

int32_t global;

void slowInc(int32_t *p, int64_t inc) {
  int64_t aux = *p;
  yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

void my_process_inc(uint8_t argc, char *argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 3) {
    return;
  }

  if ((n = satoi(argv[0])) <= 0) {
    return;
  }

  if ((inc = satoi(argv[1])) == 0) {
    return;
  }
  if ((use_sem = satoi(argv[2])) < 0) {
    return;
  }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem) {
      sem_wait(SEM_ID);
    }
    slowInc(&global, inc);
    if (use_sem) {
      sem_post(SEM_ID);
    }
  }

  return;
}

void test_sync(uint8_t argc, char *argv[]) { // { n, use_sem, 0 }
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2) {
    printf("Incorrect argument count. Expected: 2");
    return;
  }

  char *argvDec[] = {argv[0], "-1", argv[1], 0};
  char *argvInc[] = {argv[0], "1", argv[1], 0};

  global = 0;

  int8_t sem_open_result = sem_open(SEM_ID, 1);
  if (sem_open_result == -1) {
    printf("Error while opening semaphore");
    return;
  }

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = create_process(my_process_inc, 3, argvDec, "my_process_inc", 0);
    pids[i + TOTAL_PAIR_PROCESSES] =
        create_process(my_process_inc, 3, argvInc, "my_process_inc", 0);
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    wait_pid(pids[i], BLOCK);
    wait_pid(pids[i + TOTAL_PAIR_PROCESSES], BLOCK);
  }

  sem_close(SEM_ID);

  printf("test_sync: final global value %d\n", global);

  return;
}