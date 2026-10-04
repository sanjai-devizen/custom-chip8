# Define the compiler
CC = gcc

# Compiler flags (-Wall for warnings, -g for debugging symbols)
CFLAGS = -Wall -g

# Define the path to your Raylib installation
RAYLIB_PATH = ../raylib/raylib/src

# Linker flags for Raylib and native Windows dependencies
LDFLAGS = -L$(RAYLIB_PATH) -lraylib -lopengl32 -lgdi32 -lwinmm

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
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile .c files into .o object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Target to launch the program with gdbserver listening on the port
.PHONY: debug
debug: $(TARGET)
	@echo "Starting gdbserver on port $(GDB_PORT)..."
	gdbserver $(GDB_PORT) ./$(TARGET)

# Clean up compiled files (using standard Windows 'del' shell command compatibility)
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET) || del $(OBJS) $(TARGET)
