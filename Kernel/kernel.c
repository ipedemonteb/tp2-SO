#include <stdint.h>
#include <string.h>
#include "./include/moduleLoader.h"
#include "./include/naiveConsole.h"
#include "./include/idtLoader.h"
#include "./include/interrupts.h"
#include "./include/memory_manager.h"
#include "./include/scheduler.h"
#include "./include/process_manager.h"
#include "./include/sem.h"
#include "./include/test_sync.h"
#include "./include/videoDriver.h"
#include "./include/test_util.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void *)0x400000;
static void * const sampleDataModuleAddress = (void *)0x500000;

typedef int (* EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void * getStackBase() {
	return (void *)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
					- sizeof(uint64_t)					  // Begin at the top of the stack
	);
}

void * initializeKernelBinary() {
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress};

	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
	return getStackBase();
}

void init(){
	// uint8_t * argv[] = {0}; 
	// uint8_t * argv2[] = {(uint8_t *)"4", 0};
	//create_process(test_sync, 0, argv, (int8_t *)"test_prio");
	//print_process_info();
	//wait_children();
  int i=0;
  while(1) {
    i++;
  }
}

//   // drawString("abri el semaforo con el id", 0, 10, WHITE, BLACK);
//   // drawchar(sem_id + '0', 30, 10 + sem_id, WHITE, BLACK);
//   // drawString("el valor inicial del semaforo es", 0, 11, WHITE, BLACK);
//   // drawchar(get_sem_value(sem_id) + '0', 40, 11, WHITE, BLACK);
//   // sem_wait(sem_id);
//   // drawString("el valor del semaforo luego de hacer wait es", 0, 12, WHITE, BLACK);
//   // drawchar(get_sem_value(sem_id) + '0', 50, 12, WHITE, BLACK);
//   // sem_post(sem_id);
//   // drawString("el valor del semaforo luego de hacer up es", 0, 13, WHITE, BLACK);
//   // drawchar(get_sem_value(sem_id) + '0', 50, 13, WHITE, BLACK);
//   // sem_wait(sem_id);
//   // drawString("el valor del semaforo luego de hacer wait es", 0, 14, WHITE, BLACK);
//   // drawchar(get_sem_value(sem_id) + '0', 50, 14, WHITE, BLACK);
//   // sem_post(sem_id);
//   // drawString("el valor del semaforo luego de hacer up es", 0, 15, WHITE, BLACK);
//   // drawchar(get_sem_value(sem_id) + '0', 50, 15, WHITE, BLACK);
//   // sem_close(sem_id);
//   // drawString("cerre el semaforo con el id", 0, 16, WHITE, BLACK);
//   // drawchar(sem_id + '0', 30, 16 + sem_id, WHITE, BLACK);

// void inc(int64_t * p, int64_t inc) {
//   uint64_t aux = * p;
//   yield(); // This makes the race condition highly probable
//   aux += inc;
//   *p = aux;
// }

// int8_t sharedValue = '0';

// void inc_value (uint64_t argc, char *argv[]) {
//   int8_t sem_id = sem_open(1);
//   if(sem_id < 0) {
//     drawString((int8_t *)"inc_value: ERROR opening semaphore", 0, 2, RED, BLACK);
//   }
//   else {
//     drawString("inc_value: abri el semaforo con el id", 0, 3, WHITE, BLACK);
//     drawchar(sem_id + '0', 50, 3, WHITE, BLACK);
//   }
//   drawString("inc_value: el valor inicial del semaforo es", 0, 4, WHITE, BLACK);
//   drawchar(get_sem_value(0) + '0', 60, 4, RED, BLACK);
//   for (int i = 0; i< 100; i++) {
//     yield();
//   }
//   drawString("inc_value: voy a hacer sem_wait", 0, 5, WHITE, BLACK);
//   sem_wait(0);
//   drawString("inc_value: entre al wait, tengo el acceso", 0, 6, WHITE, BLACK);
//   drawString("inc_value: el valor del semaforo es", 0, 7, WHITE, BLACK);
//   drawchar(get_sem_value(0) + '0', 50, 7, RED, BLACK);
//   drawString("inc_value: sharedValue vale", 0, 8, WHITE, BLACK);
//   drawchar(sharedValue + '0', 50, 8, RED, BLACK);
//   inc(&sharedValue, 1);
//   drawString("inc_value: aumente sharedValue, vale", 0, 8, WHITE, BLACK);
//   drawchar(sharedValue + '0', 50, 8, RED, BLACK);
//   drawString("inc value: voy a esperar para hacer sem_post", 0, 9, WHITE, BLACK);
//   int k=0;
//   for (int i=0; i<10000000; i++) {
//     k++;
//   }
//   drawString("inc_value: voy a hacer sem_post", 0, 9, WHITE, BLACK);
//   sem_post(0);
//   drawString("inc_value: hice el post, libere el acceso", 0, 10, WHITE, BLACK);
//   drawString("inc_value: el valor del semaforo es", 0, 11, WHITE, BLACK);
//   drawchar(get_sem_value(0) + '0', 50, 11, RED, BLACK);
//   drawString("inc_value: termine de ejecutar", 0, 30, WHITE, BLACK);
// }

// void dec_value (uint64_t argc, char *argv[]) {
//   int8_t sem_id = sem_open(1);
//   if(sem_id < 0) {
//     drawString((int8_t *)"dec_value: ERROR opening semaphore", 0, 2, RED, BLACK);
//   }
//   else {
//     drawString("dec_value: abri el semaforo con el id", 0, 3, WHITE, BLACK);
//     drawchar(sem_id + '0', 50, 3, WHITE, BLACK);
//   }
//   drawString("dec_value: el valor inicial del semaforo es", 0, 14, WHITE, BLACK);
//   drawchar(get_sem_value(0) + '0', 60, 14, RED, BLACK);
//   drawString("dec_value: voy a hacer sem_wait", 0, 15, WHITE, BLACK);
//   sem_wait(0);
//   drawString("dec_value: entre al wait, tengo el acceso", 0, 16, WHITE, BLACK);
//   drawString("dec_value: el valor del semaforo es", 0, 17, WHITE, BLACK);
//   drawchar(get_sem_value(0) + '0', 50, 17, RED, BLACK);
//   drawString("dec_value: sharedValue vale", 0, 18, WHITE, BLACK);
//   drawchar(sharedValue + '0', 50, 18, RED, BLACK);
//   inc(&sharedValue, -1);
//   drawString("dec_value: disminui sharedValue, vale", 0, 19, WHITE, BLACK);
//   drawchar(sharedValue + '0', 50, 19, RED, BLACK);
//   drawString("dec_value: voy a hacer sem_post", 0, 20, WHITE, BLACK);
//   sem_post(0);
//   drawString("dec_value: hice el post, libere el acceso", 0, 21, WHITE, BLACK);
//   drawString("dec_value: el valor del semaforo es", 0, 22, WHITE, BLACK);
//   drawchar(get_sem_value(0) + '0', 50, 22, RED, BLACK);
//   drawString("dec_value: termine de ejecutar", 0, 31, WHITE, BLACK);
// }


// void test_sems1(uint64_t argc, char *argv[]) {
//   uint8_t * argv_test[] = {0};
//   create_process(inc_value, 1, argv_test, (int8_t *)"inc_value");
//   create_process(dec_value, 1, argv_test, (int8_t *)"dec_value");
//   drawString("test_sems1: termine de ejecutar", 0, 33, WHITE, BLACK);
// }

// void test_max_sems(uint64_t argc, char *argv[]) {
//   for (int i=0; i<40; i++) {
//     int8_t sem_id = sem_open(1);
//     drawString("test_max_sems: abri el semaforo con el id", 0, 3 + i, WHITE, BLACK);
//     char buffer[10];
//     numToStr(sem_id, buffer);
//     drawString(buffer, 60, 3 + i, GREEN, BLACK);
//   }
// }



int main() {
	load_idt();
	start_mm();
	init_scheduler(getStackBase());
	init_semaphores();
	uint8_t * argv[] = {0}; 

  //create_process(test_max_sems, 0, argv, (int8_t *) "test_max_sems");
  //create_process(test_sems1, 0, argv, (int8_t *)"test_sems1");
	uint8_t * argv_sync[] = {"5", "1", NULL};
	create_process(test_sync, 2, argv_sync, (int8_t *)"test_sync");
  create_process(init, 0, argv, (int8_t *)"init");
	_sti();
	while(1) {
	}
	return 0;
}