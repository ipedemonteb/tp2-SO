#include "sem.h"

semaphore semaphores[SEM_MAX] = {0};

extern int _xchg(int *lock, int value); // lock es el mutex y value el valor que le quiero poner
extern void acquire(int * lock);
extern void release(int * lock);

// xchg intercambia los valores de lock y value -> me devuelve el valor anterior de lock
// si el mutex estaba desbloqueado y lo quiero bloquear -> xchg(lock, 0) -> me devuelve 1
// si el mutex estaba bloqueado y lo quiero desbloquear -> xchg(lock, 1) -> me devuelve 0
// si el mutex estaba desbloqueado y lo quiero desbloquear -> xchg(lock 1) -> me devuelve 1
// si el mutex estaba bloqueado y lo quiero bloquear -> xchg(lock, 0) -> me devuelve 0

// por ende, para adquirir el mutex tengo que hacer xchg(lock, 0) y si me devuelve 0 tengo que volver a intentar 
//            -> cuando me devuelva 1 es porque lo adquiri yo y ya lo bloquee para otros

// por ende, para soltar el mutex podria hacer xchg(lock, 1) y ya estaría pero como soy el unico que tiene acceso al mutex no
// necesito que la operación sea atómica nadie puede cambiar su valor hasta que yo no lo libere -> por eso puedo hacer sem->mutex=1


void enable_list(listADT list) {      // itera por la lista y marca a los procesos como ready y los saca de la lista
  while (peek(list) != NULL) {
    uint16_t pid = (uint16_t) poll(list);
    unblock(pid);
  }
}

void acquire_sem_lock(semaphore * sem) {
  acquire(sem->sem_lock);
}

void release_sem_lock(semaphore * sem) {
  release(sem->sem_lock);
}

int8_t sem_open(int8_t id, int8_t value) {
  if(semaphores[id].value != NULL) {  // ya existe el semáforo con ese id 
    return -1;
  }
  semaphore * sem = &semaphores[id];
  semaphores[id].value = value;
  semaphores[id].sem_lock = 1;       // arranca disponible para usar
  semaphores[id].waiting_sem = new_list(NULL);
  return 0;
}

int8_t sem_close(int8_t id) {
  if (semaphores[id].value == NULL) {     // no existe el semáforo  
    return -1;
  }
  free_list(semaphores[id].waiting_sem);
  return 0;
}

int8_t sem_post(int8_t id) {
  if (semaphores[id].value == NULL) {   // no existe el semaforo 
    return -1;
  }
  acquire_sem_lock(&semaphores[id]);   // obtengo control absoluto sobre el semaforo para poder modificar el valor
  semaphores[id].value++;
  if (semaphores[id].value == 1) {  // si ahora vale 1 --> habia procesos esperando hacer que puedan correr (status = ready)
    enable_list(semaphores[id].waiting_sem);
  }
  release_sem_lock(&semaphores[id]);   // suelto el control del semaforo ya que no voy a seguir modificando el valor
  return 0;
}

int8_t sem_wait(int8_t id) {
  if (semaphores[id].value == NULL) {     // no existe el semáforo
    return -1;
  }
  acquire_sem_lock(&semaphores[id]);   // obtengo control absoluto sobre el semaforo para poder modificar su valor
  while(semaphores[id].value == 0) {   // si el valor del semaforo es 0 no puedo disminuirlo, por ende libero el sem lock y me bloqueo
    int16_t my_pid = get_current_pid();
    add(semaphores[id].waiting_sem, (void *) my_pid);     // me agrego a la lista de los que estan esperando al semaforo
    release_sem_lock(&semaphores[id]);  // suelto el control del semaforo ya que no voy a seguir modificando el valor
    block(my_pid);        // me bloqueo
    yield();            // libero el cpu

    acquire_sem_lock(&semaphores[id]);   // vuelvo a intentar acceder a valor del semaforo -> vuelvo a adquirir el sem lock
  }
  semaphores[id].value--;      // voy a salir del while cuando el valor del semaforo no sea 0, por ende puedo disminuirlo
  release_sem_lock(&semaphores[id]);   // suelto el control del semaforo ya que no voy a seguir modificando el valor
  return 0;

}