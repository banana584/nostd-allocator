#include "../include/slab.h"
#include "impls/slab_impl.h"

slab slab_create(alloc_backend backend, const size_t num_pools, const size_t num_blocks) {
    slab slab = { .backend = backend, .num_pools = num_pools };
    slab.bootstrap = arena_create(backend, sizeof(pool) * num_pools);

    slab.pools = arena_alloc(&slab.bootstrap, sizeof(pool) * num_pools);

    size_t size = 1;
    for (size_t i = 0; i < num_pools; i++) {
        slab.pools[i] = pool_create(backend, size, num_blocks);
        size <<= 1;
    }

    return slab;
}

void slab_destroy(slab* slab) {
    if (!slab) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    for (size_t i = 0; i < slab->num_pools; i++) {
        pool_destroy(&slab->pools[i]);
    }

    arena_destroy(&slab->bootstrap);
}

void* slab_alloc(slab* slab, const size_t size) {
    if (!slab) {
        alloc_err = RES_INVALID_ARG;
        return NULL;
    }

    size_t idx = log2_int(size);
    
    if (idx > slab->num_pools) {
        size_t* n = (size_t*)slab->backend.alloc(size + sizeof(size_t) + sizeof(_Bool));
        _Bool* flag = (_Bool*)((char*)n + sizeof(size_t));
        *n = size;
        *flag = 1;

        return (void*)((char*)flag + sizeof(_Bool));
    }

    pool* pool = &slab->pools[idx];
    
    size_t* n = (size_t*)pool_alloc(pool);
    _Bool* flag = (_Bool*)((char*)n + sizeof(size_t));
    *n = idx;
    *flag = 0;

    return (void*)((char*)flag + sizeof(_Bool));
}

void slab_free(slab* slab, void* ptr) {
    if (!slab || !ptr) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    size_t* n = (size_t*)((char*)ptr - sizeof(_Bool) - sizeof(size_t));
    _Bool* flag = (_Bool*)((char*)n + sizeof(size_t));

    if (*flag) {
        slab->backend.free(n, *n);
        return;
    }

    pool* pool = &slab->pools[*n];

    pool_free(pool, n);
}