# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2
INCLUDES = -Iinclude

# Directories
SRC_DIR = src
BIN_DIR = bin
DOWNLOAD_DIR = download

# Targets
TARGET = $(BIN_DIR)/download

# Source files - ADD parser.c HERE
SRC = $(SRC_DIR)/download.c $(SRC_DIR)/parser.c $(SRC_DIR)/getip.c $(SRC_DIR)/clientTCP.c

# Default target
all: $(TARGET)

# Compile the download program - UPDATED to use all source files
$(TARGET): $(SRC)
	@mkdir -p $(BIN_DIR) $(DOWNLOAD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Clean bin and download directories
clean:
	@if [ -d "$(BIN_DIR)" ]; then \
		rm -rf $(BIN_DIR)/*; \
	fi
	@if [ -d "$(DOWNLOAD_DIR)" ]; then \
		rm -rf $(DOWNLOAD_DIR)/*; \
	fi

example: $(TARGET)
	@./$(TARGET) "ftp://demo:password@test.rebex.net/readme.txt"

# Show help
help:
	@echo "Available targets:"
	@echo "  all     : Compile the download program (default)"
	@echo "  clean   : Remove all files from bin and download directories"
	@echo "  example : Compile and run the program with an example"
	@echo "  help    : Show this help message"

# Phony targets (not actual files)
.PHONY: all clean run example help