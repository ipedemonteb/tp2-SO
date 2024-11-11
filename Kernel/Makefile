include Makefile.inc

KERNEL=kernel.bin
KERNEL_ELF=$(KERNEL:.bin=.elf)
GCCFLAGS += -I./include -Wall -std=c11

ifeq ($(MEMORY_MANAGER), buddy)
	MEMORY_SOURCES=memory/buddy.c
else ifeq ($(MEMORY_MANAGER), free_array)
	MEMORY_SOURCES=memory/mmADT.c memory/memory_manager.c
endif

SOURCES=$(MEMORY_SOURCES)
SOURCES +=$(wildcard *.c) $(wildcard drivers/*.c) $(wildcard idt/*.c) $(wildcard libc/*.c) $(wildcard process/*.c) $(wildcard sync/*.c) $(wildcard pipes/*.c)


SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=loader.asm
LOADEROBJECT=$(LOADERSRC:.asm=.o)

all: $(KERNEL)

debug: all
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o $(KERNEL_ELF) $(LOADEROBJECT) $(STATICLIBS) $(OBJECTS) $(OBJECTS_ASM)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o drivers/*.o idt/*.o libc/*.o memory/*.o process/*.o sync/*.o pipes/*.o *.o *.bin *.elf

.PHONY: all clean