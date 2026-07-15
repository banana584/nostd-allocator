#ifndef NOSTDALLOC_IMPLS_POOLIMPL_H
#define NOSTDALLOC_IMPLS_POOLIMPL_H

#include "../../include/pool.h"

typedef struct pool_node {
    struct pool_node* next;
    struct pool_node* prev;
} pool_node;

typedef struct pool_impl {
    alloc_backend* backend;
    void* mem;
    size_t block_size;
    size_t num_blocks;
    pool_node* head;
} pool_impl;

#endif