#include "../include/arena.h"
#include "impls/arena_impl.h"

arena arena_create(alloc_backend* backend, const size_t cap) {
    arena_impl alloc = { .backend = backend, .cap = cap };
    alloc.mem = backend->alloc(cap);
    if (!alloc.mem) {
        alloc.cap = 0;
    }

    arena arena;
    memcpy(&arena.mem, &alloc, sizeof(alloc));

    return arena;
}

void arena_destroy(arena* arena) {
    if (!arena) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    arena_impl* alloc = (arena_impl*)arena;

    if (alloc->mem) {
        alloc->backend->free(alloc->mem, alloc->cap);
        alloc->mem = NULL;
    }

    alloc->cap = 0;
    alloc->off = 0;
}

void* arena_alloc(arena* arena, const size_t size) {
    if (!arena) {
        alloc_err = RES_INVALID_ARG;
        return NULL;
    }

    arena_impl* alloc = (arena_impl*)arena;

    size_t new_off = alloc->off + size;
    if (new_off > alloc->cap) {
        alloc_err = RES_NO_MEM;
        return NULL;
    }

    void* ptr = (void*)((char*)alloc->mem + alloc->off);
    alloc->off = new_off;
    return ptr;
}

void arena_free(arena* arena, const size_t size) {
    if (!arena) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    arena_impl* alloc = (arena_impl*)arena;

    size_t new_off = alloc->off - size;
    if (new_off > alloc->cap) {
        alloc_err = RES_NO_MEM;
        return;
    }

    alloc->off = new_off;
}

void arena_reset(arena* arena) {
    if (!arena) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    arena_impl* alloc = (arena_impl*)arena;

    alloc->off = 0;
}