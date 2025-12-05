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
SRC = $(SRC_DIR)/download.c $(SRC_DIR)/parser.c $(SRC_DIR)/getip.c $(SRC_DIR)/clientTCP.c $(SRC_DIR)/create_file.c

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

example1: $(TARGET)
	@./$(TARGET) "ftp://ftp.up.pt/pub/archlinux/archive/iso/arch-0.8-base-i686.iso"

example2: $(TARGET)
	@./$(TARGET) "ftp://demo:password@test.rebex.net/readme.txt"

example3: $(TARGET)
	@./$(TARGET) "ftp://anonymous:anonymous@ftp.bit.nl/speedtest/100mb.bin"

error1: $(TARGET)
	@./$(TARGET) "ftp://anonymous@ftp.bit.nl/speedtest/100mb.bin"

error2: $(TARGET)
	@./$(TARGET) "ftp://anonymousanonymous@ftp.bit.nl/speedtest/100mb.bin"

error3: $(TARGET)
	@./$(TARGET) "ftp://anonymous:anonymousftp.bit.nl/speedtest/100mb.bin"

error4: $(TARGET)
	@./$(TARGET) "ftp//anonymous:anonymous@ftp.bit.nl/speedtest/100mb.bin"

error5: $(TARGET)
	@./$(TARGET) "ftp:\\anonymous:anonymous@ftp.bit.nl/speedtest/100mb.bin"

error7: $(TARGET)
	@./$(TARGET) "ftp://anonymous:anonymous@/speedtest/100mb.bin"

error8: $(TARGET)
	@./$(TARGET) "ftp:///speedtest/100mb.bin"

error9: $(TARGET)
	@./$(TARGET) "ftp://anonymous:anonymous@ftp.bit.nl:21/speedtest/100mb.bin"

# Show help
help:
	@echo "Available targets:"
	@echo "  all     : Compile the download program (default)"
	@echo "  clean   : Remove all files from bin and download directories"
	@echo "  example1 : Compile and run the program with an example: ftp://ftp.up.pt/pub/gnu/emacs/elisp-manual-21-2.8.tar.gz
	@echo "  example2 : Compile and run the program with an example: ftp://demo:password@test.rebex.net/readme.txt"
	@echo "  example3 : Compile and run the program with an example: ftp://anonymous:anonymous@ftp.bit.nl/speedtest/100mb.bin
	@echo "  help    : Show this help message"