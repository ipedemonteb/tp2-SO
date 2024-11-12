#ifndef INTERRUPS_H_
#define INTERRUPS_H_

#include "idt_loader.h"

void _irq00Handler(void);
void _irq01Handler(void);
void _irq02Handler(void);
void _irq03Handler(void);
void _irq04Handler(void);
void _irq05Handler(void);
void _irq80Handler(void);
void _exception0Handler(void);
void _exception06Handler(void);
void _cli(void);
void _sti(void);
void _hlt(void);
void picMasterMask(uint8_t mask);
void picSlaveMask(uint8_t mask);
void haltcpu(void);

#endif
