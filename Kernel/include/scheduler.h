#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>
#include "memory_manager.h"
#include "listADT.h"

enum Status {READY, RUNNING, BLOCKED, KILLED};

typedef struct process_struct {
	uint16_t pid;
	void * stack_base;
	void * stack_ptr;
	uint8_t ** argv;
	char * name; // TODO: esto va o no? no seria el ptr a la funcion?
	uint8_t priority;
	enum Status status;
	void * children_processes; // todo: son void * ?? es una lista de procesos
} process_struct;

typedef struct scheduler_struct {
    listADT schedule;        // TODO: que sea una lista de nodos compuestos por pid + puntero al proximo
    uint16_t current_running_pid;

} scheduler_struct;

void init_scheduler();
void schedule();
uint16_t get_current_pid();

#endif
