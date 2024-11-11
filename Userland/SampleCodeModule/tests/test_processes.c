#include <stdint.h>
#include "../include/tests.h"
#include "../include/libc.h"

enum State { RUNNING_P,
             BLOCKED_P,
             KILLED_P };

typedef struct P_rq {
  int32_t pid;
  enum State state;
} p_rq;

void test_processes(uint8_t argc, char *argv[]) {
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes;
  char *argvAux[] = {0};

  if (argc != 1){
    printf("Incorrect argument count. Expected: 1\n");
    return;
  }

  if ((max_processes = satoi(argv[0])) <= 0){
    printf("Max processes must be greater than 0\n");
    return;
  }

  p_rq p_rqs[max_processes];
  while (1) {
    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
      p_rqs[rq].pid = create_process(endless_loop, 0, argvAux, "endless_loop",0);
      if (p_rqs[rq].pid == -1) {
        return;
      } else {
        p_rqs[rq].state = RUNNING_P;
        alive++;
      }
    }
    bussy_wait(10000000);
    // Randomly kills, blocks or unblocks processes until every one has been KILLED_P
    while (alive > 0) {
      for (rq = 0; rq < max_processes; rq++) {
        action = GetUniform(100) % 2;

        switch (action) {
          case 0:
            if (p_rqs[rq].state == RUNNING_P || p_rqs[rq].state == BLOCKED_P) {
              if (kill(p_rqs[rq].pid) == -1) {
                printf("test_processes: ERROR killing process");
                return;
              }
              p_rqs[rq].state = KILLED_P;
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == RUNNING_P) {
              if (block(p_rqs[rq].pid) == -1) {
                printf("ERROR blocking process");
                return;
              }
              p_rqs[rq].state = BLOCKED_P;
            }
            break;
        }
      }

      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCKED_P && GetUniform(100) % 2) {
          if (unblock(p_rqs[rq].pid) == -1) {
            printf("ERROR unblocking process");
            return;
          }
          p_rqs[rq].state = RUNNING_P;
        }
    }
    wait_children();
  }
}
