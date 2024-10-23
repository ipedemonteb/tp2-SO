#include "../include/test_sync.h"


#define SEM_ID 1
#define TOTAL_PAIR_PROCESSES 1

int64_t global; // shared memory

int64_t count = 0;

void slowInc(int64_t *p, int64_t inc) {
  uint64_t aux = *p;
  yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 3)
    return -1;

  if ((n = satoi(argv[0])) <= 0)
    return -1;
  if ((inc = satoi(argv[1])) == 0)
    return -1;
  if ((use_sem = satoi(argv[2])) < 0)
    return -1;

  if (use_sem) {
    count++;
    if(!sem_open(SEM_ID + count, 1)) {
      drawString((int8_t *)"test_sync: ERROR opening semaphore", 20, 20, RED, BLACK);
      return -1;
    }
  }
  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem)
      sem_wait(SEM_ID);
    slowInc(&global, inc);
    if (use_sem)
      sem_post(SEM_ID);
  }

  if(use_sem)
    sem_close(SEM_ID);
  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
  drawString((int8_t *)"Starting test_sync", 20, 20, WHITE, BLACK);
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2)
    return -1;

  char *argvDec[] = {argv[0], "-1", argv[1], NULL};
  char *argvInc[] = {argv[0], "1", argv[1], NULL};

  global = 0;

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = create_process(my_process_inc, 3, argvDec, "my_process_inc");
    pids[i + TOTAL_PAIR_PROCESSES] = create_process(my_process_inc, 3, argvInc, "my_process_inc");
  }

  drawString((int8_t *)"Llegue", 25, 25, WHITE, BLACK);

  /* for (i = 0; i < TOTAL_PAIR_PROCESSESxw; i++) {
    wait_pid(pids[i]);
    wait_pid(pids[i + TOTAL_PAIR_PROCESSES]);
  } */

  wait_children();

  drawString("Final value", 30, 30, WHITE, BLACK);
  drawString(global + "0", 45, 30, WHITE, BLACK);

  drawString("Sali del test", 30, 33, WHITE, BLACK);
  return 0;
}