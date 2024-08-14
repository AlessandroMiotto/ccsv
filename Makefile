CC = gcc
CFLAGS = -Wall

SRC_DIRS = ./
SOURCES = $(shell find $(SRC_DIRS) -name '*.c')
TARGET = main

$(TARGET) : main.c
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)


run:
	@./$(TARGET)

.PHONY: clean
clean:
	@rm $(TARGET)