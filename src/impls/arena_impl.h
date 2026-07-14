#ifndef NOSTDALLOC_IMPLS_ARENAIMPL_H
#define NOSTDALLOC_IMPLS_ARENAIMPL_H

#include "../../include/arena.h"

struct arena {
    alloc_backend backend;
    void* mem;
    size_t cap;
    size_t off;
};

#endif