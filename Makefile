CC=gcc
LIB=ar

CFLAGS=-std=c99 -Wall -Werror -Wextra -Wpedantic -ffreestanding -fshort-wchar -mno-red-zone -fno-stack-protector -Iinclude
LIBFLAGS=rcs

SOURCE_DIR=src
BUILD_DIR=build
ARTIFACTS_DIR=$(BUILD_DIR)/artifacts

SOURCES=$(SOURCE_DIR)/memutils.c $(SOURCE_DIR)/arena.c $(SOURCE_DIR)/pool.c $(SOURCE_DIR)/slab.c $(SOURCE_DIR)/buddy.c $(SOURCE_DIR)/alloc.c
OBJ=$(patsubst $(SOURCE_DIR)/%.c,$(ARTIFACTS_DIR)/%.o,$(SOURCES))

.PHONY: all clean

all: clean $(BUILD_DIR)/liballoc.a

$(ARTIFACTS_DIR)/%.o: $(SOURCE_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/liballoc.a: $(OBJ)
	mkdir -p $(dir $@)
	$(LIB) $(LIBFLAGS) $@ $^
	rm -rf $(ARTIFACTS_DIR)

clean:
	rm -rf $(BUILD_DIR)