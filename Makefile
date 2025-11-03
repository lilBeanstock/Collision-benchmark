# Directories.
SRC_DIR := src
BUILD_DIR := build

# "/src/ files.
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
HDR_FILES := $(wildcard $(SRC_DIR)/*.h)
# Converts .c files to .o files.
BUILD_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# The compiled executable binary file.
TARGET := main
# Compiler and linker flags.
CFLAGS := -Wall -Wextra -O2 -I$(SRC_DIR)
LDFLAGS := -lraylib -lm -ldl -lpthread -lGL -lX11
CC := gcc

# Default target when running `make`.
all: $(TARGET)

# Depend on all object and header files: forces relink.
$(TARGET): $(BUILD_FILES) $(HDR_FILES)
	$(CC) $(CFLAGS) -o $@ $(BUILD_FILES) $(LDFLAGS)

# Ensure the build directory exists.
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile each src/*.c -> build/*.o  files.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(BUILD_FILES)
