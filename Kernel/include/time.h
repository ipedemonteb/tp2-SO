#ifndef _TIME_H_
#define _TIME_H_
#include <stdint.h>

void setTimerTick(uint32_t frequency);
void timer_handler();
int ticks_elapsed();
int seconds_elapsed();
void timer_wait(uint32_t tick);
uint16_t getCurrentTime(uint32_t t);

#endif
