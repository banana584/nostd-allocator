#include "../include/alloc.h"

void allocator_destroy(allocator* alloc) {
    if (!alloc) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    if (alloc->destroy) {
        alloc->destroy(alloc->ctx);
        alloc->ctx = NULL;
        alloc->destroy = NULL;
    }

    alloc->alloc = NULL;
    alloc->free = NULL;
}

void* allocator_alloc(allocator* alloc, const size_t size) {
    if (!alloc) {
        alloc_err = RES_INVALID_ARG;
        return NULL;
    }

    return alloc->alloc(alloc->ctx, size);
}

void* allocator_realloc(allocator* alloc, void* ptr, const size_t size) {
    if (!alloc || !ptr) {
        alloc_err = RES_INVALID_ARG;
        return NULL;
    }

    return alloc->realloc(alloc->ctx, ptr, size);
}

void allocator_free(allocator* alloc, void* ptr) {
    if (!alloc || !ptr) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    alloc->free(alloc->ctx, ptr);
}