#include "./include/syscall.h"
#define EMPTY (void *)0

int64_t read(uint8_t bd, char *buffer, int64_t size) {
    return (int64_t) syscaller(0, (void *)(uintptr_t)bd, buffer, (void *)(uintptr_t)size, EMPTY, EMPTY);
}

int64_t write(uint8_t bd, char *buffer, int64_t size) {
    return (int64_t) syscaller(1, (void *)(uintptr_t)bd, buffer, (void *)(uintptr_t)size, EMPTY, EMPTY);
}

void printRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t colour) {
    syscaller(2, (void *)(uintptr_t)x, (void *)(uintptr_t)y, (void *)(uintptr_t)width, (void *)(uintptr_t)height, (void *)(uintptr_t)colour);
}

uint64_t getScreenWidth() {
    return (uint64_t) syscaller(3, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}

uint64_t getScreenHeight() {
    return (uint64_t) syscaller(4, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}

void printChar(uint8_t c, uint32_t x, uint32_t y, uint32_t charColour, uint32_t bgColour) {
    syscaller(5, (void *)(uintptr_t)c, (void *)(uintptr_t)x, (void *)(uintptr_t)y, (void *)(uintptr_t)charColour, (void *)(uintptr_t)bgColour);
}

void wait(int ticks) {
    syscaller(6, (void *)(uintptr_t)ticks, EMPTY, EMPTY, EMPTY, EMPTY);
}

uint8_t keyboardStatus() {
    return (uint8_t)(uintptr_t)syscaller(7, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}

uint8_t keyboardKey() {
    return (uint8_t)(uintptr_t)syscaller(8, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}

void beepSys(uint32_t frequence, int waitTicks) {
    syscaller(9, (void *)(uintptr_t)frequence, (void *)(uintptr_t)waitTicks, EMPTY, EMPTY, EMPTY);
}

uint8_t fontSizeUp() {
    return (uint8_t)(uintptr_t)syscaller(10, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}

uint8_t fontSizeDown() {
    return (uint8_t)(uintptr_t)syscaller(11, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}

void time(char bp[]) {
    syscaller(12, bp, EMPTY, EMPTY, EMPTY, EMPTY);
}

void * my_malloc(uint64_t size) {
    return syscaller(13, (void *)(uintptr_t)size, EMPTY, EMPTY, EMPTY, EMPTY);
}

void my_free(void *p) {
    syscaller(14, p, EMPTY, EMPTY, EMPTY, EMPTY);
}

int8_t create_process(void (*fn)(uint64_t, char **), uint8_t argc, char **argv, char *name, uint8_t fg) {
    return (int8_t)(uintptr_t)syscaller(15, (void *)fn, (void *)(uintptr_t)argc, (void *)argv, (void *)name, (void *)(uintptr_t)fg);
}

uint8_t kill(uint16_t pid) {
    return (uint8_t)(uintptr_t)syscaller(16, (void *)(uintptr_t)pid, EMPTY, EMPTY, EMPTY, EMPTY);
}

void my_exit() {
    syscaller(17, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}

uint8_t block(uint16_t pid) {
    return (uint8_t)(uintptr_t)syscaller(18, (void *)(uintptr_t)pid, EMPTY, EMPTY, EMPTY, EMPTY);
}

uint8_t unblock(uint16_t pid) {
    return (uint8_t)(uintptr_t)syscaller(19, (void *)(uintptr_t)pid, EMPTY, EMPTY, EMPTY, EMPTY);
}

uint8_t nice(uint16_t pid, uint8_t priority) {
    return (uint8_t)(uintptr_t)syscaller(20, (void *)(uintptr_t)pid, (void *)(uintptr_t)priority, EMPTY, EMPTY, EMPTY);
}

void yield() {
    syscaller(21, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}

uint8_t ps(process_info *info) {
    return (uint8_t)(uintptr_t)syscaller(22, (void *)info, EMPTY, EMPTY, EMPTY, EMPTY);
}

void wait_children() {
    syscaller(23, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}

uint8_t wait_pid(uint16_t pid, uint8_t block) {
    return (uint8_t)(uintptr_t)syscaller(24, (void *)(uintptr_t)pid, (void *)(uintptr_t)block, EMPTY, EMPTY, EMPTY);
}

uint16_t get_current_pid() {
    return (uint16_t)(uintptr_t)syscaller(25, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY);
}

int8_t sem_open(int8_t id, int8_t value) {
    return (int8_t)(uintptr_t)syscaller(26, (void *)(uintptr_t)id, (void *)(uintptr_t)value, EMPTY, EMPTY, EMPTY);
}

int8_t sem_close(int8_t id) {
    return (int8_t)(uintptr_t)syscaller(27, (void *)(uintptr_t)id, EMPTY, EMPTY, EMPTY, EMPTY);
}

int8_t sem_post(int8_t id) {
    return (int8_t)(uintptr_t)syscaller(28, (void *)(uintptr_t)id, EMPTY, EMPTY, EMPTY, EMPTY);
}

int8_t sem_wait(int8_t id) {
    return (int8_t)(uintptr_t)syscaller(29, (void *)(uintptr_t)id, EMPTY, EMPTY, EMPTY, EMPTY);
}

int8_t pipe(int8_t id, uint8_t pipe_bd[2]) {
    return (int8_t)(uintptr_t)syscaller(30, (void *)(uintptr_t)id, pipe_bd, EMPTY, EMPTY, EMPTY);
}

void close(uint8_t bd) {
    syscaller(31, (void *)(uintptr_t)bd, EMPTY, EMPTY, EMPTY, EMPTY);
}

void copy(uint8_t dest_bd, uint8_t source_bd) {
    syscaller(32, (void *)(uintptr_t)dest_bd, (void *)(uintptr_t)source_bd, EMPTY, EMPTY, EMPTY);
}

void key_to_screen(uint8_t flag) {
    syscaller(33, (void *)(uintptr_t)flag, EMPTY, EMPTY, EMPTY, EMPTY);
}

void swap(uint8_t bd1, uint8_t bd2) {
    syscaller(34, (void *)(uintptr_t)bd1, (void *)(uintptr_t)bd2, EMPTY, EMPTY, EMPTY);
}

void mem_data(mem_info * info) {
    syscaller(35, (void *) info, EMPTY, EMPTY, EMPTY, EMPTY);
}