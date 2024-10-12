#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define STACKSIZE   4096

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
	struct process_struct * parent_pcb;
	uint64_t children_processes[2];
	uint64_t killed_children[2];
} process_struct;

#endif