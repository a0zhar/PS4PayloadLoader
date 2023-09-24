# This directory will contain the compiled payload (.elf and .bin) files.
OUT_DIR := build

# This directory contains the C implementation of a firmware-agnostic
# sandbox escaper designed for the PS4 system. Contains a combination of
# .h and .c files as well as the jbc.o file.
JBC_DIR := libJBC

# This directory contains all FreeBSD v9.0.0 related headers required
# to create payloads and programs for the PS4 system. Contains only .h
# files.
FREEBSD9_INCLUDE := freebsd-headers

# This directory contains a C project: a custom syscall for extended
# symbol resolving on the PS4 (allowing specification of library name
# and flags to use mangled symbol names). Contains .o/.c files + lib.a.
LIB_DIR := lib

# The compiler that will be used.
CC := gcc

all: $(OUT_DIR)/payload.bin

clean:
	@echo "Cleaning up..."
	@rm -rf $(OUT_DIR)
	@echo "Cleanup complete."

$(LIB_DIR)/lib.a:
	@echo "Building the library..."
	@cd $(LIB_DIR); make
	@echo "Library built successfully."
	@echo "Building JBC library..."
	@cd $(JBC_DIR); make
	@echo "JBC library built successfully."

$(OUT_DIR)/payload.elf: $(LIB_DIR)/lib.a main.c
	@if [ ! -d "$(OUT_DIR)" ]; then mkdir $(OUT_DIR); fi
	@echo "Compiling $<..."
	@$(CC) -isystem $(FREEBSD9_INCLUDE) -nostdinc -nostdlib -fno-stack-protector -static $(LIB_DIR)/lib.a main.c $(JBC_DIR)/*.c -Wl,-gc-sections \
		   -o $(OUT_DIR)/payload.elf -fPIE -ffreestanding
	@echo "Compilation completed successfully."

$(OUT_DIR)/payload.bin: $(OUT_DIR)/payload.elf
	@echo "Creating binary payload..."
	@objcopy $(OUT_DIR)/payload.elf --only-section .text --only-section .data --only-section .bss --only-section .rodata -O binary $(OUT_DIR)/payload.bin
	@file $(OUT_DIR)/payload.bin | fgrep -q 'payload.bin: DOS executable (COM)'
	@echo "Binary payload created!"
	@echo "Generating JS File..."
	@bash makeJS.sh
	@echo "JS File generated successfully."
