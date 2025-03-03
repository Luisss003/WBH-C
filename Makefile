# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -std=c11 `pkg-config --cflags cairo`

# Linker flags
LDFLAGS = `pkg-config --libs cairo`

# Source files
SRCS = main.c

# Object files
OBJS = $(SRCS:.c=.o)

# Output executable
TARGET = wbh-c

# Default rule
all: $(TARGET)

# Link the final executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean run

