#include "mmADT.h"
#include <unistd.h>
#define BYTE 8

char bits[BYTE] = { 1 , 2 , 4 , 8 , 16 , 32 , 64 , 128};

struct mmCDT{
    void * start;
    uint64_t free;
    uint64_t size;
    char * bitmap;
};

mmADT init_mm(void * start, uint64_t size){
    
}
void * mm_alloc(mmADT mmCDT , uint64_t size){

}