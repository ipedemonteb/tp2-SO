#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

enum Status {READY, BLOCKED, KILLED};

typedef struct process_struct {
	uint16_t pid;
	void * stack_base;
	void * stack_ptr;
	uint8_t ** argv;
	char * name; // TODO: esto va o no? no seria el ptr a la funcion?
	uint8_t priority;
    uint8_t count; // TODO: ver que onda
	enum Status status;
    uint8_t blocked_in;
	void * children_processes; // todo: son void * ?? es una lista de procesos
} process_struct;

#endif