#include "../include/idtLoader.h"
#include "../include/defs.h"
#include "../include/interrupts.h"

#pragma pack(push) // push of current alignment
#pragma pack(1)    // align following structures to one byte

typedef struct {
  uint16_t offset_l, selector;
  uint8_t cero, access;
  uint16_t offset_m;
  uint32_t offset_h, other_cero;
} DESCR_INT;  // interruption descriptor struct

#pragma pack(pop) // rest old alignment

DESCR_INT * idt = (DESCR_INT *) 0; // idt of 255 entries

void setup_IDT_entry(uint32_t index, uint64_t offset) {
  idt[index].selector = 0x08;
  idt[index].offset_l = offset & 0xFFFF;
  idt[index].offset_m = (offset >> 16) & 0xFFFF;
  idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
  idt[index].access = ACS_INT;
  idt[index].cero = 0;
  idt[index].other_cero = (uint64_t)0;
}

void load_idt() {
  setup_IDT_entry(0x00, (uint64_t)&_exception0Handler);
  setup_IDT_entry(0x06, (uint64_t)&_exception06Handler);
  setup_IDT_entry(0x80, (uint64_t)&_irq80Handler);
  setup_IDT_entry(0x20, (uint64_t)&_irq00Handler);
  setup_IDT_entry(0x21, (uint64_t)&_irq01Handler);
  picMasterMask(0xFC);
  picSlaveMask(0xFF);
  _sti();
}