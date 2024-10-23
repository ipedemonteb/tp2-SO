#include "./include/syscall.h"
#define EMPTY 0

uint32_t read(uint8_t * buffer, uint32_t size){
    return syscaller(0, buffer, size, EMPTY, EMPTY, EMPTY);
}
long int write(uint8_t fd, const uint8_t * string, uint32_t size){
    return syscaller(1, fd, string, size, EMPTY, EMPTY);
}
void printRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t colour){
    syscaller(2, x, y, width, height, colour);
}
uint64_t getScreenWidth(){
    return syscaller(3, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}
uint64_t getScreenHeight(){
    return syscaller(4, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}
void printChar(uint8_t c, uint32_t x, uint32_t y, uint32_t charColour, uint32_t bgColour){
    syscaller(5, c, x, y, charColour, bgColour);
}
void wait(int ticks){
    syscaller(6, ticks, EMPTY, EMPTY, EMPTY, EMPTY);
}
uint8_t keyboardStatus(){
    return syscaller(7, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}
uint8_t keyboardKey(){
    return syscaller(8, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}
void beepSys(uint32_t frequence, int waitTicks){
    syscaller(9, frequence, waitTicks, EMPTY, EMPTY, EMPTY);
}
uint8_t fontSizeUp(){
    return syscaller(10, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}
uint8_t fontSizeDown(){
    return syscaller(11, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);

}
void time(int8_t bp[]){
    return syscaller(12, bp, EMPTY, EMPTY, EMPTY, EMPTY);
}

void * my_malloc(uint64_t size){
    return syscaller(13,size, EMPTY, EMPTY, EMPTY, EMPTY);
}
void my_free(void * p) {
    syscaller(14, p, EMPTY, EMPTY, EMPTY, EMPTY);
}

int8_t create_process(void (*fn)(uint8_t, uint8_t **), uint8_t argc, uint8_t **argv, int8_t *name) {
    return syscaller(15, fn, argc, argv, name, EMPTY);
}

uint8_t kill(uint16_t pid) {
    return syscaller(16, pid, EMPTY, EMPTY, EMPTY, EMPTY);
}

void my_exit() {
    syscaller(17, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}

uint8_t block(uint16_t pid) {
    return syscaller(18, pid, EMPTY, EMPTY, EMPTY, EMPTY);
}

uint8_t unblock(uint16_t pid) {
    return syscaller(19, pid, EMPTY, EMPTY, EMPTY, EMPTY);
}

uint8_t nice(uint16_t pid, uint8_t priority) {
    return syscaller(20, pid, priority, EMPTY, EMPTY, EMPTY);
}

void yield() {
    syscaller(21, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}

uint8_t ps(process_info *info) {
    return syscaller(22, info, EMPTY, EMPTY, EMPTY, EMPTY);
}

void wait_children() {
    syscaller(23, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}

uint16_t get_current_pid() {
    return syscaller(24, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}