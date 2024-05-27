SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
HEADERS   := $(wildcard $(SRC_DIR)/*.h)

CC        := clang
CFLAGS    := -g -Wall -Wextra -pedantic
LDFLAGS   := -lm

# compile source files into object files in obj/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ensure obj/ directory exists
$(OBJ_DIR):
	mkdir -p $@

# ensure bin/ directory exists
$(BIN_DIR):
	mkdir -p $@

# link object files to create the binary in bin/
$(BIN_DIR)/main: $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ -o $@

.PHONY: all clean

# default target
all: $(BIN_DIR)/main

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
