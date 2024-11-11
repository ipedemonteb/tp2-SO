#include <stdint.h>
#include "./include/moduleLoader.h"
#include "./include/naiveConsole.h"
#include "./include/idtLoader.h"
#include "./include/interrupts.h"
#include "./include/memory_manager.h"
#include "./include/scheduler.h"
#include "./include/process_manager.h"
#include "./include/test_sync.h"
#include "./include/keyboardDriver.h"
#include "./include/terminal_driver.h"
#include "./include/sem.h"
#include "./include/memory_manager.h"
#include "./include/mmADT.h"


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

void init_memory_manager() {
  #ifdef MEMORY_MANAGER_BUDDY
    init_buddy();
  #elif defined(MEMORY_MANAGER_FREE_ARRAY)
    start_mm();
  #endif
}

int main() {
	start_keyboard_driver();
	start_terminal();
	load_idt();
	//_sti();
	//((EntryPoint)sampleCodeModuleAddress)();
	init_memory_manager();
	init_scheduler(getStackBase());
	init_semaphores();
	uint8_t * argv[] = {0}; 
	create_first_process((void *)sampleCodeModuleAddress,0, argv, "init");
  //create_first_process((void *) test_buddy, 0, argv, "test_buddy");
	_sti();
	while(1) {
	}
	return 0;
}