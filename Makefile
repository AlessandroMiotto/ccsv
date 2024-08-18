SRC_DIRS = ./src
BUILD_DIR = ./build
INC_DIR = ./include

CC = icx
CFLAGS = -Wall -I $(INC_DIR) -lm 
OPENMPFLAGS = -fopenmp
MKLFLAGS = -lmkl_intel_ilp64 -lmkl_intel_thread -lmkl_core -liomp5 -lpthread -ldl -qopenmp

SOURCES = $(shell find $(SRC_DIRS) -name '*.c')
TARGET = main

$(BUILD_DIR)/$(TARGET) : $(SOURCES)
	@rm -rf $(BUILD_DIR)
	@mkdir $(BUILD_DIR)
	$(CC) $(SOURCES) -o $(BUILD_DIR)/$(TARGET) $(CFLAGS) $(MKLFLAGS)


run:
	@$(BUILD_DIR)/$(TARGET)

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)