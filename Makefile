# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Iinclude -Wall

# Linker flags (libraries to link)
LDFLAGS = -luuid -lcjson

# Source files
SRCS = src/main.c

# Output binary name
TARGET = build/out

# Default target: build the program
all: $(TARGET)

# Rule to build the program
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Clean up build artifacts
clean:
	rm -f $(TARGET)

