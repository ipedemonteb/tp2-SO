#include "../include/test_sync.h"

#define TOTAL_PAIR_PROCESSES    3
#define SEM_ID                  1

int64_t global; // shared memory
int64_t debug_sync = 5;
int k = 0;

void slowInc(int64_t * p, int64_t inc) {
  int64_t aux = * p;
  char auxx[10];
  numToStr(aux, auxx);
  drawString(auxx, k, 35, RED, BLACK);
  yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
  numToStr(aux, auxx);
  drawString(auxx, k, 37, RED, BLACK);
  k += 3;

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

  if((n = satoi(argv[0])) <= 0) {  // me fijo si n esta bien
    return -1;
  }

  if((inc = satoi(argv[1])) == 0) {  // me fijo si inc es un valor distinto de cero
    return -1;
  }
  if((use_sem = satoi(argv[2])) < 0) { // me fijo si use_sem esta bien (que sea 0 o algo positivo)
    return -1;
  }

  // if(use_sem) {
  //   int8_t sem_open_result = sem_open(SEM_ID, 1);
  //   if(sem_open_result == -1) {
  //     return -1;
  //   }
  // }

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

  drawString("my_process_inc: starting", 0, 8 + debug_sync, WHITE, BLACK);
  char buf[10], buf2[10];
  numToStr(get_current_pid(), buf);
  numToStr(inc, buf2);
  drawString(buf, 30, 8 + debug_sync, GREEN, BLACK);
  drawString(buf2, 35, 8 + debug_sync, GREEN, BLACK);

  // if(use_sem) {
  //   sem_close(SEM_ID);
  // }

  drawString("my_process_inc final value", 0, 9 + debug_sync, WHITE, BLACK);
  char buf3[10], buf4[10];
  numToStr(global, buf3);
  drawString(buf3, 30, 9 + debug_sync, GREEN, BLACK);

  debug_sync += 3;
  return 0;
}

/*
argc = 2 --> solo usamos 2 argumentos, n y use_sem

argv[0] = n --> indica la cantidad de veces que se va a modificar el valor global por proceso (-1*n o 1*n)
argv[1] = use_sem --> indica si se va a usar semaforos o no (0 o 1)
argv[2] = 0  --> (NULL)
*/
uint64_t test_sync(uint64_t argc, char * argv[]) { // { n, use_sem, 0 }
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  drawString("test_sync: starting", 0, 2, WHITE, BLACK);

  if(argc != 2) {
    return -1;
  }

  char * argvDec[] = {argv[0], "-1", argv[1], NULL};
  char * argvInc[] = {argv[0], "1", argv[1], NULL};

  global = 0;

  int8_t buffer[10];
  numToStr(global, buffer);
  drawString("test_sync: global value: ", 0, 3, WHITE, BLACK);
  drawString(buffer, 25, 3, GREEN, BLACK);

  sem_open(SEM_ID, 1);

  // creo procesos: 0 2 1 3 -> dec inc dec inc -> - 2 + 2 - 2 + 2 (pids = 1 q 2 r)
  uint64_t i;
  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = create_process(my_process_inc, 3, argvDec, "my_process_inc");
    drawString("test_sync: created process dec with pid", 0, 4 + i, WHITE, BLACK);
    char buffer[10];
    numToStr(pids[i], buffer);
    drawString(buffer, 40, 4 + i, GREEN, BLACK);
    pids[i + TOTAL_PAIR_PROCESSES] = create_process(my_process_inc, 3, argvInc, "my_process_inc");
    drawString("test_sync: created process inc with pid", 0, 4 + i + TOTAL_PAIR_PROCESSES, WHITE, BLACK);
    numToStr(pids[i + TOTAL_PAIR_PROCESSES], buffer);
    drawString(buffer, 40, 4 + i + TOTAL_PAIR_PROCESSES, GREEN, BLACK);
  }

  wait_children();

  sem_close(SEM_ID);
  char final[10];
  numToStr(global, final);
  drawString("test_sync: final global value: ", 0, 45, WHITE, BLACK);
  drawString(final, 50, 45, GREEN, BLACK);

  return 0;
}