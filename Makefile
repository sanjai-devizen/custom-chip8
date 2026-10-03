# Define the compiler
CC = gcc

# Compiler flags (-Wall for warnings, -g for debugging symbols)
CFLAGS = -Wall -g -c

# Define the final executable name
TARGET = main

# Define the source files and object files
SRCS = main.c chip8.c
OBJS = $(SRCS:.c=.o)

# GDB Port for remote debugging
GDB_PORT = :1234

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Compile .c files into .o object files
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# Target to launch the program with gdbserver listening on the port
.PHONY: debug
debug: $(TARGET)
	@echo "Starting gdbserver on port $(GDB_PORT)..."
	gdbserver $(GDB_PORT) ./$(TARGET)

# Clean up compiled files
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
