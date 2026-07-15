#ifndef NOSTDALLOC_INCLUDE_SLAB_H
#define NOSTDALLOC_INCLUDE_SLAB_H

#include <stddef.h>
#include "memutils.h"
#include "arena.h"
#include "pool.h"

typedef struct slab {
    unsigned char mem[56UL];
} slab;

slab slab_create(alloc_backend* backend, const size_t num_pools, const size_t num_blocks);

void slab_destroy(slab* slab);

void* slab_alloc(slab* slab, const size_t size);

void slab_free(slab* slab, void* ptr);

#endif