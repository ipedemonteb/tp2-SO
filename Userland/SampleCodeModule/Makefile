include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
MODULE_ELF=$(MODULE:.bin=.elf)
SOURCES=$(wildcard [^_]*.c) $(wildcard shell/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=_loader.c
LOADEROBJECT=$(LOADERSRC:.c=.o)

all: $(MODULE)

debug: all
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 -o ../$(MODULE_ELF) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM)
$(MODULE): $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld -o ../$(MODULE) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM)
%.o: %.c
	$(GCC) $(GCCFLAGS)  -I./include -c $< -o $@
%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf asm/*.o *.o

.PHONY: all clean print
