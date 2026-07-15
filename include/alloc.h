#ifndef NOSTDALLOC_INCLUDE_ALLOC_H
#define NOSTDALLOC_INCLUDE_ALLOC_H

#include "arena.h"
#include "pool.h"
#include "slab.h"
#include "buddy.h"

typedef void (*destroy_func)(void*);
typedef void* (*alloc_func)(void*, const size_t);
typedef void (*free_func)(void*, void*);

typedef struct allocator {
    void* ctx;
    destroy_func destroy;
    alloc_func alloc;
    free_func free;
} allocator;

void allocator_destroy(allocator* alloc);

void* allocator_alloc(allocator* alloc, const size_t size);

void allocator_free(allocator* alloc, void* ptr);

#endif