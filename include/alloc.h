#ifndef NOSTDALLOC_INCLUDE_ALLOC_H
#define NOSTDALLOC_INCLUDE_ALLOC_H

#include "arena.h"
#include "pool.h"
#include "slab.h"
#include "buddy.h"

typedef void (*destroy_func)(void*);
typedef void* (*alloc_func)(void*, const size_t);
typedef void* (*realloc_func)(void*, void*, const size_t);
typedef void (*free_func)(void*, void*);

typedef struct allocator {
    void* ctx;
    destroy_func destroy;
    alloc_func alloc;
    realloc_func realloc;
    free_func free;
} allocator;

#define allocator_destroy(alloc) ((alloc)->destroy((alloc)->ctx))

#define allocator_alloc(alloc, size) ((alloc)->alloc((alloc)->ctx, size))

#define allocator_realloc(alloc, ptr, size) ((alloc)->realloc((alloc)->ctx, ptr, size))

#define allocator_free(alloc, ptr) ((alloc)->free((alloc)->ctx, ptr))

#endif