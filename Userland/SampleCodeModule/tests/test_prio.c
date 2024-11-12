// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/libc.h"
#include "../include/tests.h"
#include <stdint.h>

#define MINOR_WAIT                                                             \
  10000000 // TODO: Change this value to prevent a process from flooding the
           // screen
#define WAIT                                                                   \
  100000000 // TODO: Change this value to make the wait long enough to see
            // theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 1  // TODO: Change as required
#define MEDIUM 2  // TODO: Change as required
#define HIGHEST 3 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio(uint8_t argc, char *argv[]) {
  int64_t pids[TOTAL_PROCESSES];
  char *argv1[] = {"1000000", 0};
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] =
        create_process(endless_loop_print, 1, argv1, (char *)"funcion", 0);

  bussy_wait(WAIT);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    nice(pids[i], prio[i]);

  bussy_wait(WAIT);

  printf("BLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    block(pids[i]);

  printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    nice(pids[i], MEDIUM);

  printf("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    unblock(pids[i]);

  bussy_wait(WAIT);
  printf("KILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    kill(pids[i]);
}
