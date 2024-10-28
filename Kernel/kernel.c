#include <stdint.h>
#include "./include/moduleLoader.h"
#include "./include/naiveConsole.h"
#include "./include/idtLoader.h"
#include "./include/interrupts.h"
#include "./include/memory_manager.h"
#include "./include/scheduler.h"
#include "./include/process_manager.h"
#include "./include/test_sync.h"
#include "include/sem.h"


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
	uint8_t * argv[] = {0}; 
	uint8_t * argv2[] = {(uint8_t *)"4", 0};
	//create_process(halt_cpu, 0, argv, "inactive");
	//create_process(test_prio, 0, argv, (int8_t *)"test_prio");
	//create_process(test_processes, 1, argv2, (int8_t *)"test_proc");
	create_process(sampleCodeModuleAddress,0, argv, (int8_t *)"Terminal");
	//print_process_info();
	wait_children();
}

int main() {
	load_idt();
	//_sti();
	//((EntryPoint)sampleCodeModuleAddress)();
	start_mm();
	init_scheduler(getStackBase());
	init_semaphores();
	uint8_t * argv[] = {0}; 
	create_process((void *)sampleCodeModuleAddress,0, argv, (int8_t*)"init");
	_sti();
	while(1) {
	}
	return 0;
}