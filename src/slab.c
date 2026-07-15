#include "../include/slab.h"
#include "impls/slab_impl.h"

slab slab_create(alloc_backend* backend, const size_t num_pools, const size_t num_blocks) {
    slab_impl alloc = { .backend = backend, .num_pools = num_pools };
    alloc.bootstrap = arena_create(backend, sizeof(pool) * num_pools);

    alloc.pools = arena_alloc(&alloc.bootstrap, sizeof(pool) * num_pools);

    size_t size = 1ULL;
    for (size_t i = 0; i < num_pools; i++) {
        alloc.pools[i] = pool_create(backend, size, num_blocks);
        size <<= 1ULL;
    }

    slab slab;
    memcpy(&slab.mem, &alloc, sizeof(alloc));

    return slab;
}

void slab_destroy(slab* slab) {
    if (!slab) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    slab_impl* alloc = (slab_impl*)slab;

    for (size_t i = 0; i < alloc->num_pools; i++) {
        pool_destroy(&alloc->pools[i]);
    }

    arena_destroy(&alloc->bootstrap);
}

void* slab_alloc(slab* slab, const size_t size) {
    if (!slab) {
        alloc_err = RES_INVALID_ARG;
        return NULL;
    }

    slab_impl* alloc = (slab_impl*)slab;

    size_t idx = log2_int(size);
    
    if (idx > alloc->num_pools) {
        size_t* n = (size_t*)alloc->backend->alloc(size + sizeof(size_t) + sizeof(_Bool));
        _Bool* flag = (_Bool*)((char*)n + sizeof(size_t));
        *n = size;
        *flag = 1;

        return (void*)((char*)flag + sizeof(_Bool));
    }

    pool* pool = &alloc->pools[idx];
    
    size_t* n = (size_t*)pool_alloc(pool);
    _Bool* flag = (_Bool*)((char*)n + sizeof(size_t));
    *n = idx;
    *flag = 0;

    return (void*)((char*)flag + sizeof(_Bool));
}

void* slab_realloc(slab* slab, void* ptr, const size_t size) {
    if (!slab || !ptr) {
        alloc_err = RES_INVALID_ARG;
        return NULL;
    }

    slab_impl* alloc = (slab_impl*)slab;

    size_t* old = (size_t*)((char*)ptr - sizeof(_Bool) - sizeof(size_t));

    arena scratch = arena_create(alloc->backend, *old);
    void* mem = arena_alloc(&scratch, *old);
    memcpy(mem, ptr, *old);

    slab_free(slab, ptr);

    void* new = slab_alloc(slab, size);
    memcpy(new, mem, MIN(*old, size));

    arena_destroy(&scratch);

    return new;
}

void slab_free(slab* slab, void* ptr) {
    if (!slab || !ptr) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    slab_impl* alloc = (slab_impl*)slab;

    size_t* n = (size_t*)((char*)ptr - sizeof(_Bool) - sizeof(size_t));
    _Bool* flag = (_Bool*)((char*)n + sizeof(size_t));

    if (*flag) {
        alloc->backend->free(n, *n);
        return;
    }

    pool* pool = &alloc->pools[*n];

    pool_free(pool, n);
}