include Makefile.inc

SAMPLE_DATA=0001-sampleDataModule.bin

all: sampleCodeModule sampleDataModule

debug: all
	cd SampleCodeModule; make debug

sampleCodeModule:
	cd SampleCodeModule; make

sampleDataModule:
	printf "This is sample data." >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)

clean:
	cd SampleCodeModule; make clean
	rm -rf *.bin *.elf


.PHONY: sampleCodeModule all clean
