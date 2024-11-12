// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "./include/idt_loader.h"
#include "./include/interrupts.h"
#include "./include/keyboard_driver.h"
#include "./include/memory_manager.h"
#include "./include/module_loader.h"
#include "./include/process_manager.h"
#include "./include/scheduler.h"
#include "./include/sem.h"
#include "./include/terminal_driver.h"
#include <stdint.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t end_of_kernel_binary;
extern uint8_t end_of_kernel;

static const uint64_t page_size = 0x1000;

static void *const sample_code_module_address = (void *)0x400000;
static void *const sample_data_module_address = (void *)0x500000;

typedef int (*entry_point)();

void clear_BSS(void *bss_address, uint64_t bss_size) {
  memset(bss_address, 0, bss_size);
}

void *get_stack_base() {
  return (void *)((uint64_t)&end_of_kernel + page_size * 8 - sizeof(uint64_t));
}

void *initialize_kernel_binary() {
  void *module_addresses[] = {sample_code_module_address,
                              sample_data_module_address};

  load_modules(&end_of_kernel_binary, module_addresses);
  clear_BSS(&bss, &end_of_kernel - &bss);
  return get_stack_base();
}

int main() {
  start_keyboard_driver();
  start_terminal();
  load_idt();
  start_mm();
  init_scheduler(get_stack_base());
  init_semaphores();
  char *argv[] = {0};
  create_first_process((void *)sample_code_module_address, 0, argv, "init");
  _sti();
  while (1) {
  }
  return 0;
}