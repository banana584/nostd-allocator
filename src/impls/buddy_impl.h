#ifndef NOSTDALLOC_IMPLS_BUDDYIMPL_H
#define NOSTDALLOC_IMPLS_BUDDYIMPL_H

#include "../../include/buddy.h"

typedef struct buddy_node {
    size_t order;
    _Bool free;
    struct buddy_node* next;
    struct buddy_node* prev;
} buddy_node;

typedef struct buddy_impl {
    alloc_backend* backend;
    size_t max_order;
    size_t max_size;
    void* mem;
    buddy_node* head;
} buddy_impl;

#endif