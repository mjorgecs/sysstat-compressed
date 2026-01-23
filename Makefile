# Compiler flags
CC = gcc

CFLAGS = -Wall -Wextra -std=c99 -I../sysstat-repo/

# Target names and source files
TARGET = decompressor

TARGET_FILE = target.bin

SRC_FILES = $(TARGET).c wr_stats.c rd_stats.c helper.c pr_stats.c ../sysstat-repo/activity.c

FILE = file2.bin

OBJS = $(SRC_FILES:.c=.o)

# Phony targets
.PHONY: all build verbose run clean help

# Default target (when 'make' is run without arguments call help)
all: help

# Default build
build: $(TARGET)

# Build with debug flags
verbose: CFLAGS += -DVERBOSE -g

verbose: $(TARGET)

read_file:
	$(CC) $(CFLAGS) read_file.c -o read_file -lm && ./read_file $(FILE)

# Compile rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link normal executable
# $@ is the target name (in this case, reader)
# $^ is the list of dependencies (OBJS)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET) $(FILE) > out4.txt

run-comp:
	./$(TARGET) $(FILE) $(TARGET_FILE)

clean:
	rm -f *.o $(TARGET)

help:
	@echo "Makefile commands:"
	@echo "  make build		- Compile the program"
	@echo "  make verbose  	- Compile the program with verbose flags"
	@echo "  make run      	- Run the compiled program"
	@echo "  make clean    	- Remove compiled files"
	@echo "  make read_file	- Compile and run read_file program"
	@echo "  make help     	- Show this help message"