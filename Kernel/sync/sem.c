#include "../include/sem.h"
#include <stdint.h>

semaphore semaphores[SEM_MAX] = {0};

extern int _xchg(int8_t * lock, int8_t value);
extern void acquire(int8_t * lock);
extern void release(int8_t * lock);

void init_semaphores() {
  for (int i = 0; i < SEM_MAX; i++) {
    semaphores[i].value = NOT_INIT;
    semaphores[i].sem_lock = 1;
  }
}

void enable_list(listADT list) {
  while (peek(list) != NULL) {
    uint16_t pid = (uint16_t)(uintptr_t) poll(list);
    unblock(pid);
  }
}

int8_t enable_first(listADT list) {
  if (peek(list) == NULL) {
    return -1;
  }
  uint16_t pid = (uint16_t)(uintptr_t) poll(list);
  unblock(pid);
  return 0;
}

void acquire_sem_lock(semaphore * sem) {
  acquire(&sem->sem_lock);
}

void release_sem_lock(semaphore * sem) {
  release(&sem->sem_lock);
}

int8_t sem_open(int8_t id, int8_t value) {
  if (id >= SEM_MAX || id <= 0) {
    return -1;
  }

  acquire_sem_lock(&semaphores[id]);
  if (semaphores[id].value != NOT_INIT) {
      release_sem_lock(&semaphores[id]);
      return 0;
  }

  semaphores[id].value = value;
  semaphores[id].sem_lock = 1;
  semaphores[id].waiting_sem = new_list(NULL);
  release_sem_lock(&semaphores[id]);
  return id;
}

int8_t sem_close(int8_t id) {
  if (id >= SEM_MAX || id <= 0 || semaphores[id].value == NOT_INIT) {
    return -1;
  }
  acquire_sem_lock(&semaphores[id]);
  enable_list(semaphores[id].waiting_sem);
  free_list(semaphores[id].waiting_sem);
  semaphores[id].value = NOT_INIT;
  release_sem_lock(&semaphores[id]);
  return 0;
}

int8_t sem_post(int8_t id) {
  if (id >= SEM_MAX || id <= 0 || semaphores[id].value == NOT_INIT) {
    return -1;
  }
  
  acquire_sem_lock(&semaphores[id]);

  if(enable_first(semaphores[id].waiting_sem) == -1) {
    semaphores[id].value++;
  }  

  release_sem_lock(&semaphores[id]);
  return 0;
}

int8_t sem_wait(int8_t id) {
  if (id >= SEM_MAX || id <= 0 || semaphores[id].value == NOT_INIT) {
    return -1;
  }

  acquire_sem_lock(&semaphores[id]);
  int16_t my_pid = get_current_pid();

  if(semaphores[id].value == 0) {
    add(semaphores[id].waiting_sem, (void *)(uintptr_t) my_pid);
    release_sem_lock(&semaphores[id]);
    block(my_pid);
    return 0;
  }

  semaphores[id].value--;
  release_sem_lock(&semaphores[id]);
  return 0;
}

int8_t get_sem_value(int8_t id) {
  return semaphores[id].value;
}
