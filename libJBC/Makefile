TARGET := jbc.O

.PHONY: all clean

all: $(TARGET)

$(TARGET): *.c *.h
	@echo "Compiling $<..."
	@gcc *.c -nostdlib -nostartfiles -ffreestanding -mno-red-zone -no-pie -Wl,-r -o $(TARGET) -g
	@echo "$< compiled successfully."

clean:
	@echo "Cleaning libJBC..."
	@rm -f $(TARGET)
	@echo "Cleaned!"
