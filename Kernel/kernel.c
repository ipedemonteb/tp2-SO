#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <audioDriver.h>
#include <videoDriver.h>
#include <idtLoader.h>
#include <time.h>
#include "./include/process_manager.h"
#include "./include/test_mm.h"
#include "./include/test_process.h"
#include "./include/test_util.h"
#include "./include/interrupts.h"
#include "./include/memory_manager.h"
#include "include/videoDriver.h"


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

void test1(){
	uint64_t i = 0;
	uint8_t j = 0;
	while(1){
		i++;
		if ((i % 100000000) == 0)
		{
			drawchar(get_current_pid() + '0' , 0 , (j * 3) + 1  , WHITE , BLACK );
			j++;
		}
	}
}

void test2(){
	uint64_t i = 0;
	uint8_t j = 0;
	while(1){
		i++;
		if ((i % 100000000) == 0)
		{
			drawchar(get_current_pid() + '0' , 0 , (j * 3) , WHITE , BLACK );
			j++;
		}
	}
}

void init(){
	uint8_t * argv[] = {"16" , 0}; 
	//create_process(halt_cpu,0,argv);
	create_process(test_processes,1,argv, "test_pro");
	create_process(test_prio,1,argv, "test_prio");
	//create_process(test1,0,argv);
	//create_process(test2,0,argv);
	//create_process(sampleCodeModuleAddress,0,argv);
	wait_children();
}

int main() {
	load_idt();
	start_mm();
	init_scheduler(getStackBase());
	uint8_t * argv[] = {0}; 
	create_process(init,0,argv, "init");
	print_process_info();
	_sti();
	((EntryPoint)sampleCodeModuleAddress)();
	return 0;
}