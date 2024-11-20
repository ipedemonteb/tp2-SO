// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/process_manager.h"
#include "../include/lib.h"
#include "../include/memory_manager.h"
#include "../include/pipes.h"
#include "../include/terminal_driver.h"

#include <stdint.h>

#define ALIGN 7

uint8_t proc_count = 0;
process_struct processes[QUANT] = {0};

void *stacks = (void *)0x1000000;

uint64_t occupied[2];

void my_exit() {
  uint16_t pid = get_current_pid();
  kill(pid);
}

void launch_process(void (*fn)(uint8_t, char **), uint8_t argc, char *argv[]) {
  fn(argc, argv);
  my_exit();
}

void load_proc_stack(process_struct *p_struct, void *stack) {
  p_struct->stack_base = stack;
  p_struct->priority = 3;
  p_struct->status = READY;
  p_struct->parent_pcb = &processes[get_current_pid()];

  p_struct->parent_pcb->children_processes[(p_struct->pid) / 64] = set_n_bit_64(
      p_struct->parent_pcb->children_processes[(p_struct->pid) / 64],
      (p_struct->pid) % 64);
  p_struct->children_processes[0] = 0;
  p_struct->children_processes[1] = 0;
  p_struct->killed_children[0] = 0;
  p_struct->killed_children[1] = 0;
  p_struct->count = 1;

  for (uint8_t i = 0; i < 4; i++) {
    p_struct->open_buffers[i] = p_struct->parent_pcb->open_buffers[i];
    if (p_struct->open_buffers[i] != -1) {
      uint8_t id = p_struct->open_buffers[i] / 2;
      pipe_t *p = get_pipe_by_id(id);
      if (p_struct->open_buffers[i] % 2 == OUT) {
        p->opened_write++;
      } else {
        p->opened_read++;
      }
    }
  }

  for (uint8_t i = 4; i < MAX_BUFFERS; i++) {
    p_struct->open_buffers[i] = -1;
  }

  process_stack *p_stack = stack - sizeof(process_stack);
  p_stack->rsp = stack;
  p_stack->rbp = stack;
  p_stack->cs = (void *)0x8;
  p_stack->rflags = (void *)0x202;
  p_stack->ss = 0x0;
  p_stack->rip = launch_process;
  p_struct->stack_ptr = p_stack;
}

void load_proc_args(void (*fn)(uint8_t, char **), uint8_t argc, char *argv[],
                    void *stack) {
  process_stack *p_stack = stack - sizeof(process_stack);
  p_stack->rdi = fn;
  p_stack->rsi = (void *)(uintptr_t)argc;
  p_stack->rdx = argv;
}

int32_t create_process(void (*fn)(uint8_t, char **), uint8_t argc, char *argv[],
                       const char *name, uint8_t fg) {
  if (proc_count >= QUANT) {
    return -1;
  }
  uint16_t pid = find_off_bit_128(occupied[0], occupied[1]);
  occupied[pid / 64] = set_n_bit_64(occupied[pid / 64], pid % 64);
  process_struct *p_struct = &processes[pid];

  char **args = my_malloc((argc + 1) * sizeof(char *));
  for (uint8_t i = 0; i < argc; i++) {
    uint64_t len = my_strlen(argv[i]) + 1;
    args[i] = my_malloc(len);
    memcpy(args[i], argv[i], len);
  }
  args[argc] = NULL;

  p_struct->pid = pid;
  p_struct->argv = args;

  p_struct->fg = fg;
  if (fg) {
    add_to_fg(pid);
  }

  p_struct->name = my_malloc(my_strlen(name) + 1);
  memcpy(p_struct->name, name, my_strlen(name) + 1);
  void *stack = stacks + STACKSIZE * (pid + 1);
  load_proc_stack(p_struct, stack);
  load_proc_args(fn, argc, argv, stack);
  schedule_process(p_struct);
  return p_struct->pid;
}

void create_first_process(void (*fn)(uint8_t, char **), uint8_t argc,
                          char *argv[], const char *name) {
  process_struct *p_struct = &processes[0];
  p_struct->pid = 0;
  occupied[0] |= 1;
  p_struct->argv = argv;
  p_struct->name = my_malloc(my_strlen(name) + 1);
  memcpy(p_struct->name, name, my_strlen(name));
  void *stack = stacks + STACKSIZE;

  p_struct->stack_base = stack;
  p_struct->priority = 3;
  p_struct->status = READY;
  p_struct->parent_pcb = &processes[0];

  p_struct->count = 1;

  for (uint8_t i = 0; i < MAX_BUFFERS; i++) {
    p_struct->open_buffers[i] = i < 4 ? i : -1;
  }
  pipe_t *pipe = get_keyboard_buffer();
  pipe->opened_read++;
  pipe->opened_write++;
  pipe = get_terminal_buffer();
  pipe->opened_read++;
  pipe->opened_write++;

  process_stack *p_stack = stack - sizeof(process_stack);
  p_stack->rsp = stack;
  p_stack->rbp = stack;
  p_stack->cs = (void *)0x8;
  p_stack->rflags = (void *)0x202;
  p_stack->ss = 0x0;
  p_stack->rip = launch_process;
  p_struct->stack_ptr = p_stack;
  p_stack->rdi = fn;
  p_stack->rsi = (void *)(uintptr_t)argc;
  p_stack->rdx = argv;
  schedule_process(p_struct);
}

int8_t kill(uint16_t pid) {
  if (pid >= QUANT) {
    return -1;
  }
  processes[pid].status = KILLED;
  for (uint8_t i = 0; i < MAX_BUFFERS * 2; i++) {
    internal_close(&processes[pid], i);
  }
  if (processes[pid].fg) {
    rm_from_fg(pid);
  }

  uint8_t idx = pid / 64;
  processes[pid].parent_pcb->killed_children[idx] =
      set_n_bit_64(processes[pid].parent_pcb->killed_children[idx], pid % 64);
  if (processes[pid].parent_pcb->status == BLOCKED) {
    unblock(processes[pid].parent_pcb->pid);
  }

  uint64_t aux[] = { processes[pid].children_processes[0], processes[pid].children_processes[1] };
  uint8_t child;
  while( aux[0] || aux[1] ) {
    child = find_off_bit_128( ~aux[0] , ~aux[1]);
    aux[child / 64] = off_n_bit_64( aux[child / 64], child % 64);
    processes[child].parent_pcb = processes[pid].parent_pcb;
  }

  processes[pid].parent_pcb->children_processes[0] |=
      processes[pid].children_processes[0];
  processes[pid].parent_pcb->children_processes[1] |=
      processes[pid].children_processes[1];
  processes[pid].parent_pcb->killed_children[0] |=
      processes[pid].killed_children[0];
  processes[pid].parent_pcb->killed_children[1] |=
      processes[pid].killed_children[1];
  char **argv = processes[pid].argv;
  while (*argv) {
    my_free(*argv);
    argv++;
  }
  my_free(processes[pid].argv);
  if (get_current_pid() == pid) {
    yield();
  } else {
    deschedule_process(&processes[pid]);
  }
  return 0;
}

int8_t block(uint16_t pid) {
  if (pid >= QUANT) {
    return -1;
  }
  processes[pid].status = BLOCKED;
  if (get_current_pid() == pid) {
    yield();
  } else {
    deschedule_process(&processes[pid]);
  }
  return 0;
}

int8_t unblock(uint16_t pid) {
  if (pid >= QUANT) {
    return -1;
  }
  if (processes[pid].status == BLOCKED) {
    processes[pid].status = READY;
    schedule_process(&processes[pid]);
  }
  return 0;
}

void nice(uint16_t pid, uint8_t priority) {
  if (priority > 4) {
    return;
  }
  processes[pid].priority = priority;
}

void yield() { int20(); }

uint8_t ps(process_info *info) {
  uint8_t process_count = 0;
  for (int i = 0; i < QUANT; i++) {
    if (occupied[i / 64] & (1UL << (i % 64))) {
      process_struct *p = &processes[i];
      info[process_count].name = my_malloc(my_strlen(p->name) + 1);
      memcpy(info[process_count].name, p->name, my_strlen(p->name) + 1);
      info[process_count].pid = p->pid;
      info[process_count].priority = p->priority;
      info[process_count].stack_base = p->stack_base;
      info[process_count].stack_ptr = p->stack_ptr;
      info[process_count].foreground = p->fg;
      info[process_count].status = p->status;
      process_count++;
    }
  }
  return process_count;
}

void clear_child(process_struct *parent_pcb, uint16_t pid) {
  parent_pcb->children_processes[pid / 64] =
      off_n_bit_64(parent_pcb->children_processes[pid / 64], pid % 64);
  parent_pcb->killed_children[pid / 64] =
      off_n_bit_64(parent_pcb->killed_children[pid / 64], pid % 64);
  occupied[pid / 64] = off_n_bit_64(occupied[pid / 64], pid % 64);
  my_free(processes[pid].name);
}

void wait_children_no_block(process_struct * pcb) {
  uint8_t child;
  while (pcb->killed_children[0] || pcb->killed_children[1]) {
      child =
          find_off_bit_128(~pcb->killed_children[0], ~pcb->killed_children[1]);
      clear_child(pcb, child);
    }
}

void wait_children_block(process_struct * pcb) {
  while (pcb->children_processes[0] || pcb->children_processes[1]) {
    wait_children_no_block(pcb);
    if (pcb->children_processes[0] || pcb->children_processes[1]) {
      pcb->status = BLOCKED;
      yield();
    }
  }
}

void wait_children(uint8_t block) {
  uint16_t pid = get_current_pid();
  process_struct *pcb = &processes[pid];
  if (block) {
    wait_children_block(pcb);
  } else {
    wait_children_no_block(pcb);
  }
  
}

uint8_t wait_pid_no_block(uint16_t pid) {
  process_struct *parent_pcb = &processes[get_current_pid()];
  if (!(parent_pcb->children_processes[pid / 64] & (1UL << (pid % 64)))) {
    return 0;
  }
  if ((parent_pcb->killed_children[pid / 64] & (1UL << (pid % 64)))) {
    clear_child(parent_pcb, pid);
    return 1;
  }

  return 0;
}

uint8_t wait_pid_block(uint16_t pid) {
  process_struct *parent_pcb = &processes[get_current_pid()];
  while (!wait_pid_no_block(pid)) {
    parent_pcb->status = BLOCKED;
    yield();
  }

  return 1;
}

uint8_t wait_pid(uint16_t pid, uint8_t block) {
  if (block)
    return wait_pid_block(pid);
  else
    return wait_pid_no_block(pid);
}

process_struct *get_process_info(uint8_t pid) { return &processes[pid]; }