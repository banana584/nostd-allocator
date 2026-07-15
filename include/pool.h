#ifndef NOSTDALLOC_INCLUDE_POOL_H
#define NOSTDALLOC_INCLUDE_POOL_H

#include <stddef.h>
#include "memutils.h"

typedef struct pool {
    unsigned char mem[40UL];
} pool;

pool pool_create(alloc_backend* backend, const size_t block_size, const size_t num_blocks);

void pool_destroy(pool* pool);

void* pool_alloc(pool* pool);

void pool_free(pool* pool, void* ptr);

#endif