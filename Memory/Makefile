CC=gcc
CFLAGS=-Wall -Wextra -g
SOURCES=test_wrapper.c test_mm.c test_util.c syscall.c memory_manager.c mmADT.c
OBJECTS =$(SOURCES:.c=.o)
TARGET=memory

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -I./include -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)