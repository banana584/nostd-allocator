#include "../include/pool.h"
#include "impls/pool_impl.h"

pool* pool_create(alloc_backend backend, const size_t block_size, const size_t num_blocks) {
    pool* pool = backend.alloc(sizeof(pool));
    pool->backend = backend;
    pool->block_size = MIN(block_size, sizeof(pool_node));
    pool->num_blocks = num_blocks;
    pool->mem = backend.alloc(pool->block_size * num_blocks);
    if (!pool->mem) {
        pool->block_size = 0;
        pool->num_blocks = 0;
        return pool;
    }

    pool_node* prev = NULL;
    pool_node* cur = (pool_node*)pool->mem;
    for (size_t i = 0; i < num_blocks; i++) {
        cur->prev = prev;
        if (prev) prev->next = cur;

        cur = (pool_node*)((char*)cur + pool->block_size);
    }

    return pool;
}

void pool_destroy(pool* pool) {
    if (!pool) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    if (pool->mem) {
        pool->backend.free(pool->mem, pool->block_size * pool->num_blocks);
        pool->mem = NULL;
    }

    pool->head = NULL;
    pool->block_size = 0;
    pool->num_blocks = 0;
}

void* pool_alloc(pool* pool) {
    if (!pool) {
        alloc_err = RES_INVALID_ARG;
        return NULL;
    }

    void* ptr = (void*)pool->head;
    pool->head = pool->head->next;

    return ptr;
}

void pool_free(pool* pool, void* ptr) {
    if (!pool || !ptr) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    pool_node* node = (pool_node*)ptr;

    if (!pool->head) {
        pool->head = node;
        node->next = NULL;
        node->prev = NULL;
        return;
    }

    node->next = pool->head;
    node->prev = NULL;
    pool->head->prev = node;
    pool->head = node;
}