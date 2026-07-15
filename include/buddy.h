#ifndef NOSTDALLOC_INCLUDE_BUDDY_H
#define NOSTDALLOC_INCLUDE_BUDDY_H

#include <stddef.h>
#include "memutils.h"
#include "arena.h"

typedef __UINTPTR_TYPE__ uintptr_t;

typedef struct buddy {
    unsigned char mem[40UL];
} buddy;

buddy buddy_create(alloc_backend* backend, const size_t size);

void buddy_destroy(buddy* buddy);

void* buddy_alloc(buddy* buddy, const size_t size);

void* buddy_realloc(buddy* buddy, void* ptr, const size_t size);

void buddy_free(buddy* buddy, void* ptr);

#endif