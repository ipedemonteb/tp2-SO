#ifndef MY_SEMAPHORES_H_
#define MY_SEMAPHORES_H_

#include <stdint.h>
#include "listADT.h"
#include "scheduler.h"
#include "process_manager.h"
#include "lib.h"
#include "memory_manager.h"

#define SEM_MAX 127
#define NOT_INIT -1

typedef struct semaphore {
    int8_t value;           
    int8_t sem_lock;        
    listADT waiting_sem;    
} semaphore;

int8_t sem_open(int8_t id, int8_t value);
int8_t sem_close(int8_t id);
int8_t sem_post(int8_t id);
int8_t sem_wait(int8_t id);
int8_t get_sem_value(int8_t id);  // @todo: borrar (es para debuggeo)
void init_semaphores();

#endif