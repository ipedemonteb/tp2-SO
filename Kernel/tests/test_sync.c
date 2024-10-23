#include "test_sync.h"


#define SEM_ID 1
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

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

  if (use_sem)
    if (!sem_open(SEM_ID, 1)) {
      drawString("test_sync: ERROR opening semaphore\n", 20, 20, RED, BLACK);
      return -1;
    }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem)
      sem_wait(SEM_ID);
    slowInc(&global, inc);
    if (use_sem)
      sem_post(SEM_ID);
  }

  if (use_sem)
    sem_close(SEM_ID);

  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
  drawString("Starting test_sync", 20, 20, WHITE, BLACK);
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2)
    return -1;

  char *argvDec[] = {argv[0], "-1", argv[1], NULL};
  char *argvInc[] = {argv[0], "1", argv[1], NULL};

  global = 0;

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = create_process(&my_process_inc, 3, argvDec, "my_process_inc");
    pids[i + TOTAL_PAIR_PROCESSES] = create_process(&my_process_inc, 3, argvInc, "my_process_inc");
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    wait_children();
  }
  drawString("Final value", 30, 30, WHITE, BLACK);
  drawString(global + '0', 40, 40, WHITE, BLACK);

  return 0;
}