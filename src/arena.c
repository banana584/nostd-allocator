#include "../include/arena.h"
#include "impls/arena_impl.h"

arena* arena_create(alloc_backend backend, const size_t cap) {
    arena* arena = backend.alloc(sizeof(arena));
    arena->backend = backend;
    arena->cap = cap;
    arena->mem = backend.alloc(cap);
    if (!arena->mem) {
        arena->cap = 0;
        return arena;
    }

    return arena;
}

void arena_destroy(arena* arena) {
    if (!arena) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    if (arena->mem) {
        arena->backend.free(arena->mem, arena->cap);
        arena->mem = NULL;
    }

    arena->cap = 0;
    arena->off = 0;
}

void* arena_alloc(arena* arena, const size_t size) {
    if (!arena) {
        alloc_err = RES_INVALID_ARG;
        return NULL;
    }

    size_t new_off = arena->off + size;
    if (new_off > arena->cap) {
        alloc_err = RES_NO_MEM;
        return NULL;
    }

    void* ptr = (void*)((char*)arena->mem + arena->off);
    arena->off = new_off;
    return ptr;
}

void arena_reset(arena* arena) {
    if (!arena) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    arena->off = 0;
}