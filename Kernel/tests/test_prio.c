#include <stdint.h>
#include <stdio.h>
#include "syscall.h"
#include "test_util.h"
#include "../include/videoDriver.h"
#include "../include/process_manager.h"

#define MINOR_WAIT 10000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 100000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 1  // TODO: Change as required
#define MEDIUM 2  // TODO: Change as required
#define HIGHEST 3 // TODO: Change as required

uint8_t y = 0;
int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char * argv[] = {"1000000",0};
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = create_process(endless_loop_print, 1, argv);

  bussy_wait(WAIT);
  drawString("\nCHANGING PRIORITIES...\n",0,y++,WHITE,BLACK);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    nice(pids[i], prio[i]);

  bussy_wait(WAIT);
  drawString("\nBLOCKING...\n",0,y++,WHITE,BLACK);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    block(pids[i]);

  drawString("CHANGING PRIORITIES WHILE BLOCKED...\n",0,y++,WHITE,BLACK);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    nice(pids[i], MEDIUM);

  drawString("UNBLOCKING...\n",0,y++,WHITE,BLACK);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    unblock(pids[i]);

  bussy_wait(WAIT);
  drawString("\nKILLING...\n",0,y++,WHITE,BLACK);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    kill(pids[i]);
}
