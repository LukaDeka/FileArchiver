SRC_DIR := src
OBJ_DIR := obj

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

# link object files to create the binary in bin/
main: $(OBJ_FILES)
	$(CC) $(LDFLAGS) $^ -o $@

.PHONY: all clean

clean:
	rm -rf $(OBJ_DIR)
	rm -f main *.txt *.bmp *.data *.huff *.zip
