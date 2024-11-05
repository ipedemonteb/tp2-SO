#include "../include/time.h"
#include <stdint.h>
#include "../include/keyboardDriver.h"
#include "../include/syscalls.h"
#include "../include/pipes.h"
#include "../include/process_manager.h"
#include "../include/memory_manager.h"
#include "../include/scheduler.h"
#include "../include/sem.h"

static void int_20();
static void int_21();

typedef void * (*syscall)(void *, void *, void *, void *, void *);

void (* interrupts_arr[])(void) = {int_20, int_21};
void * (* sys_call_arr[])(void * rsi, void * rdx, void * rcx, void * r8, void * r9) = {
	(syscall)read /*0*/, (syscall)write /*1*/, (syscall)printRectangle /*2*/, (syscall)getScreenWidth /*3*/, (syscall)getScreenHeight /*4*/, 
	(syscall)printChar /*5*/, (syscall)wait /*6*/, (syscall)keyboardStatus /*7*/, (syscall)keyboardKey /*8*/, (syscall)beepSyscall /*9*/, 
	(syscall)fontSizeUp /*10*/, (syscall)fontSizeDown /*11*/, (syscall)getTime /*12*/, (syscall)my_malloc /*13*/, (syscall)my_free /*14*/, 
	(syscall)create_process /*15*/, (syscall)kill /*16*/, (syscall)my_exit /*17*/, (syscall)block /*18*/, (syscall)unblock /*19*/, (syscall)nice /*20*/,
	(syscall)yield /*21*/, (syscall)ps /*22*/, (syscall)wait_children /*23*/,(syscall)wait_pid /*24*/, (syscall)get_current_pid /*25*/, (syscall)sem_open /*26*/, 
	(syscall)sem_close /*27*/, (syscall)sem_post /*28*/, (syscall)sem_wait /*29*/, (syscall)pipe /*30*/, (syscall)close /*31*/, (syscall)copy /*32*/, (syscall)key_to_screen
};

void irqDispatcher(uint64_t irq) {
	interrupts_arr[irq]();
}

void sysCallDispatcher(uint64_t id, void * rsi, void * rdx, void * rcx, void * r8, void * r9) {
	sys_call_arr[id](rsi, rdx, rcx, r8, r9);
}

void int_20() {
	timer_handler();
}

void int_21() {
	keyboard_handler();
}