CC := gcc
AS := yasm

.PHONY: all clean

all: lib.a

clean:
	@echo "Cleaning up..."
	@rm -f lib.a *.o syscalls.asm
	@echo "Cleanup complete."

dl.o: dl.c
	@echo "Compiling dl.c..."
	@$(CC) -c -isystem ../freebsd-headers -nostdinc -fno-stack-protector -fPIE -ffreestanding dl.c -o dl.o
	@echo "dl.c compiled successfully."

crt.o: crt.asm
	@echo "Assembling crt.asm..."
	@$(AS) -f elf64 crt.asm -o crt.o
	@echo "crt.asm assembled successfully."

syscalls.o: syscalls.asm
	@echo "Assembling syscalls.asm..."
	@$(AS) -f elf64 syscalls.asm -o syscalls.o
	@echo "syscalls.asm assembled successfully."

ifneq ($(wildcard syscalls.asm syscalls.o),)
syscalls.asm: ;
else
syscalls.asm: syscalls.py
	@echo "Generating syscalls.asm..."
	@python3 syscalls.py > syscalls.asm
	@echo "syscalls.asm generated successfully."
endif

lib.a: crt.o dl.o syscalls.o
	@echo "Creating lib.a..."
	@ld -r crt.o dl.o syscalls.o -o lib.a
	@echo "lib.a created successfully."