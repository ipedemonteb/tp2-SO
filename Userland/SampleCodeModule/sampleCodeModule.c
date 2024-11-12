#include <stdint.h>
#include "./include/shell.h"
#include "./include/syscall.h"

void inactive() {
	nice(get_current_pid(),0);
	while (1);
}

int main() {
	char * argv[] = {0};
	create_process((void *)inactive, 0 , argv, "inactive", 0);
	create_process((void *)launchShell, 0, argv,"Shell", 0);
	wait_children();
  return 0;
}