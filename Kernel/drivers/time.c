#include "../include/time.h"
#include "../include/lib.h"
#include "../include/interrupts.h"

static unsigned long ticks = 0;

void timer_handler() {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 1000;
}

void timer_wait(uint32_t tick) {
	_sti();
	int now = ticks_elapsed();
	while (ticks_elapsed() - now < tick) {
	}
	return;
}
