MEMORY_MANAGER ?= free_array

all:  bootloader kernel userland toolchain image
buddy: MEMORY_MANAGER=buddy
buddy: bootloader kernel userland toolchain image

debug: all
	cd Kernel; make debug
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
	cd Memory; make clean

.PHONY: toolchain bootloader image collections kernel userland all clean
