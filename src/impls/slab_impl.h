#ifndef NOSTDALLOC_IMPLS_SLABIMPL_H
#define NOSTDALLOC_IMPLS_SLABIMPL_H

#include "arena_impl.h"
#include "pool_impl.h"

typedef struct slab_impl {
    alloc_backend* backend;
    arena bootstrap;
    size_t num_pools;
    pool* pools;
} slab_impl;

#endif