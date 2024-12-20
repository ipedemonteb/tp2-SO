#ifndef SYSCALL_H
#define SYSCALL_H
#include <stdint.h>

#define NO_BLOCK 0
#define BLOCK 1

#define STDIN 0
#define STDOUT 1
#define TERMINAL 3

#define MAX_PID 127

#define ANON -1

#define READ_END 0
#define WRITE_END 1

enum Status { READY, BLOCKED, KILLED };

typedef struct process_info {
  char *name;
  uint16_t pid;
  uint8_t priority;
  char *stack_base;
  char *stack_ptr;
  uint8_t foreground;
  enum Status status;
} process_info;

typedef struct mem_info {
  uint64_t total;
  uint64_t used;
  uint64_t free;
} mem_info;

void *syscaller(uint8_t id, void *rsi, void *rdx, void *rcx, void *r8,
                void *r9);

int64_t read(uint8_t bd, char *buffer, int64_t size);
int64_t write(uint8_t bd, char *buffer, int64_t size);
void printRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                    uint32_t color);
uint64_t getScreenWidth();
uint64_t getScreenHeight();
void printChar(uint8_t c, uint32_t x, uint32_t y, uint32_t charColor,
               uint32_t bgColour);
void wait(int ticks);
uint8_t keyboardStatus();
uint8_t keyboardKey();
void beepSys(uint32_t frequence, int waitTicks);
uint8_t fontSizeUp();
uint8_t fontSizeDown();
void time(char bp[]);
void *my_malloc(uint64_t size);
void my_free(void *p);
int8_t create_process(void (*fn)(uint8_t, char **), uint8_t argc, char **argv,
                      char *name, uint8_t fg);
int8_t kill(uint16_t pid);
int8_t block(uint16_t pid);
int8_t unblock(uint16_t pid);
int8_t nice(uint16_t pid, uint8_t priority);
void my_exit();
void yield();
uint8_t ps(process_info *info);
void wait_children();
uint8_t wait_pid(uint16_t pid, uint8_t block);
uint16_t get_current_pid();
int8_t sem_open(int8_t id, int8_t value);
int8_t sem_close(int8_t id);
int8_t sem_post(int8_t id);
int8_t sem_wait(int8_t id);
int8_t pipe(int8_t id, uint8_t pipe_bd[2]);
void close(uint8_t bd);
void copy(uint8_t dest_bd, uint8_t source_bd);
void key_to_screen(uint8_t flag);
void swap(uint8_t bd1, uint8_t bd2);
void mem_data(mem_info *info);
#endif