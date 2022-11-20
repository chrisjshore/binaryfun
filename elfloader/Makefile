all: loader elf

loader: loader.c
	gcc-11 -m32 -g -Wall -o bin/loader loader.c -ldl

elf: elf.c
	gcc-11 -m32 -pie -fPIE -o bin/elf elf.c -lm
