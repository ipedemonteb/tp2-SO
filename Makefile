
all:  bootloader kernel userland image
debug: all
	cd Kernel; make debug
	cd Userland; make debug

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean
	cd Memory; make clean

.PHONY: bootloader image collections kernel userland all clean
