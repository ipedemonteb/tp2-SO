free_array: MEMORY_MANAGER = free_array
free_array: all
buddy: MEMORY_MANAGER = buddy
buddy: all

all: bootloader kernel userland toolchain image

debug_buddy:
	make buddy
	cd Kernel; make MEMORY_MANAGER=buddy debug
	cd Userland; make debug

debug_free_array: 
	make free_array
	cd Kernel; make MEMORY_MANAGER=free_array debug
	cd Userland; make debug

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make MEMORY_MANAGER=$(MEMORY_MANAGER) all

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

toolchain:
	cd Toolchain; make all

clean:
	cd Toolchain; make clean
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: toolchain bootloader image collections kernel userland all clean
