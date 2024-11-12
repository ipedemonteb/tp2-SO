// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "./include/shell.h"
#include "./include/syscall.h"
#include <stdint.h>

void inactive() {
  nice(get_current_pid(), 0);
  while (1)
    ;
}

int main() {
  char *argv[] = {0};
  create_process((void *)inactive, 0, argv, "inactive", 0);
  create_process((void *)launchShell, 0, argv, "Shell", 0);
  wait_children();
  return 0;
}