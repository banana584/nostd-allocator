#include "../include/pool.h"
#include "impls/pool_impl.h"

pool pool_create(alloc_backend* backend, const size_t block_size, const size_t num_blocks) {
    pool_impl alloc = { .backend = backend, .block_size = MAX(block_size, sizeof(pool_node)), .num_blocks = num_blocks };
    alloc.mem = backend->alloc(alloc.block_size * num_blocks);
    if (!alloc.mem) {
        alloc.block_size = 0;
        alloc.num_blocks = 0;
        goto end;
    }

    pool_node* prev = NULL;
    pool_node* cur = (pool_node*)alloc.mem;
    alloc.head = cur;
    for (size_t i = 0; i < num_blocks; i++) {
        cur->prev = prev;
        if (prev) prev->next = cur;

        prev = cur;
        cur = (pool_node*)((char*)cur + alloc.block_size);
    }

    end: ;
    pool pool;
    memcpy(&pool.mem, &alloc, sizeof(alloc));

    return pool;
}

void pool_destroy(pool* pool) {
    if (!pool) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    pool_impl* alloc = (pool_impl*)pool;

    if (alloc->mem) {
        alloc->backend->free(alloc->mem, alloc->block_size * alloc->num_blocks);
        alloc->mem = NULL;
    }

    alloc->head = NULL;
    alloc->block_size = 0;
    alloc->num_blocks = 0;
}

void* pool_alloc(pool* pool) {
    if (!pool) {
        alloc_err = RES_INVALID_ARG;
        return NULL;
    }

    pool_impl* alloc = (pool_impl*)pool;

    void* ptr = (void*)alloc->head;
    alloc->head = alloc->head->next;

    return ptr;
}

void pool_free(pool* pool, void* ptr) {
    if (!pool || !ptr) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    pool_impl* alloc = (pool_impl*)pool;

    pool_node* node = (pool_node*)ptr;

    if (!alloc->head) {
        alloc->head = node;
        node->next = NULL;
        node->prev = NULL;
        return;
    }

    node->next = alloc->head;
    node->prev = NULL;
    alloc->head->prev = node;
    alloc->head = node;
}