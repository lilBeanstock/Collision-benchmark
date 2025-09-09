SRC = main.c
BUILD = main.o

SRC_DIR = src
BUILD_DIR = build

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
BUILD_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

TARGET = main

CFLAGS = -Wall -Wextra -O2 -I$(SRC_DIR)
LDFLAGS = -lraylib -lm -ldl -lpthread -lGL -lX11

all: $(TARGET)

./build:
	mkdir -p ./build

$(BUILD_FILES): $(SRC_FILES) | $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@

$(TARGET): $(BUILD_FILES)
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(BUILD_FILES)

.PHONY: all clean