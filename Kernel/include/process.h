#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define STACKSIZE   4096
#define MAX_BUFFERS 64

enum Status {READY, BLOCKED, KILLED};

typedef struct process_struct {
	uint16_t pid;
	void * stack_base;
	void * stack_ptr;
	uint8_t ** argv;
	char * name;
	uint8_t priority;
  	uint8_t count;
	enum Status status;
	struct process_struct * parent_pcb;
	uint64_t children_processes[2];
	uint64_t killed_children[2];
	int8_t open_buffers[MAX_BUFFERS * 2];
  	uint64_t blocked_in;
} process_struct;

typedef struct process_info {
	char * name;
	uint16_t pid;
	uint8_t priority;
	void * stack_base;
	void * stack_ptr;
	uint8_t foreground;
	enum Status status;
} process_info;

process_struct * get_process_info(uint8_t pid);

#endif