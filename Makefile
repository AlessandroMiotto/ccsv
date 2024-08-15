SRC_DIRS = ./src
BUILD_DIR = ./build
INC_DIR = ./include

CC = gcc
CFLAGS = -Wall -I $(INC_DIR) -lm

SOURCES = $(shell find $(SRC_DIRS) -name '*.c')
TARGET = main

$(BUILD_DIR)/$(TARGET) : $(SOURCES)
	@rm -rf $(BUILD_DIR)
	@mkdir $(BUILD_DIR)
	$(CC) $(SOURCES) -o $(BUILD_DIR)/$(TARGET) $(CFLAGS)


run:
	@$(BUILD_DIR)/$(TARGET)

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)