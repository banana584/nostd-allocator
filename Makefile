CC=gcc
LIB=ar

CFLAGS=-std=c11 -Wall -Werror -Wextra -Wpedantic -ffreestanding -fshort-wchar -mno-red-zone -fno-stack-protector -Iinclude -g
LIBFLAGS=rcs

SOURCE_DIR=src
BUILD_DIR=build

SOURCES=$(SOURCE_DIR)/memutils.c $(SOURCE_DIR)/arena.c $(SOURCE_DIR)/pool.c $(SOURCE_DIR)/slab.c $(SOURCE_DIR)/buddy.c
OBJ=$(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

.PHONY: all clean

all: clean $(BUILD_DIR)/liballoc.a

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/liballoc.a: $(OBJ)
	mkdir -p $(dir $@)
	$(LIB) $(LIBFLAGS) $@ $^

clean:
	rm -rf $(BUILD_DIR)