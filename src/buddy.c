#include "../include/buddy.h"
#include "impls/buddy_impl.h"

static size_t round_up(size_t n) {
    if (n == 0) return 0;

    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    #if __SIZE_WIDTH__ == 64
    n |= n >> 32;
    #endif
    n++;

    return n;
}

buddy buddy_create(alloc_backend* backend, const size_t size) {
    buddy_impl alloc = { .backend = backend, .max_size = round_up(size), .max_order = log2_int(round_up(size)) };
    alloc.mem = backend->alloc(alloc.max_size);
    if (!alloc.mem) {
        alloc.max_size = 0;
        alloc.max_order = 0;
        goto end;
    }

    buddy_node* cur = (buddy_node*)alloc.mem;
    cur->order = alloc.max_order;
    cur->free = 1;
    cur->next = NULL;
    cur->prev = NULL;

    alloc.head = cur;

    end: ;
    buddy buddy;
    memcpy(&buddy.mem, &alloc, sizeof(alloc));

    return buddy;
}

void buddy_destroy(buddy* buddy) {
    if (!buddy) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    buddy_impl* alloc = (buddy_impl*)buddy;

    if (alloc->mem) {
        alloc->backend->free(alloc->mem, alloc->max_size);
        alloc->mem = NULL;
        alloc->head = NULL;
        alloc->max_order = 0;
        alloc->max_size = 0;
    }
}

void* buddy_alloc(buddy* buddy, const size_t size) {
    if (!buddy) {
        alloc_err = RES_INVALID_ARG;
        return NULL;
    }

    buddy_impl* alloc = (buddy_impl*)buddy;

    size_t n = log2_int(round_up(size));

    buddy_node* cur = alloc->head;
    while (cur->order - 1 >= n) {
        if (!cur->free && cur->order > n) continue;

        cur->order--;

        buddy_node* other = (buddy_node*)((char*)cur + sizeof(buddy_node) + (1ULL << cur->order));
        other->next = cur->next;
        if (cur->next) cur->next->prev = other;
        cur->next = other;
        other->prev = cur;

        other->order = cur->order;
        other->free = 1;
    }

    alloc->head = cur->next;

    if (cur->prev) {
        cur->prev->next = cur->next;
    }
    if (cur->next) {
        cur->next->prev = cur->prev;
    }
    cur->prev = NULL;
    cur->next = NULL;

    cur->free = 0;

    size_t* order = (size_t*)cur;
    *order = n;
    return (void*)((char*)cur + sizeof(size_t));
}

void buddy_free(buddy* buddy, void* ptr) {
    if (!buddy || !ptr) {
        alloc_err = RES_INVALID_ARG;
        return;
    }

    buddy_impl* alloc = (buddy_impl*)buddy;

    size_t* order = (size_t*)((char*)ptr - sizeof(size_t));
    buddy_node* cur = (buddy_node*)order;
    cur->order = *order;
    cur->free = 1;
    cur->next = NULL;
    cur->prev = NULL;

    while (1) {
        uintptr_t off = (uintptr_t)cur - (uintptr_t)buddy->mem;

        uintptr_t buddy_off = off ^ (1ULL << cur->order);
        buddy_node* other = (buddy_node*)((char*)buddy->mem + buddy_off);

        if (cur->order != other->order) break;

        if (other->free) {
            buddy_node* prev = cur->prev;
            buddy_node* next = cur->next;
            if (prev) {
                cur->prev->next = next->next;
            }
            if (next) {
                cur->next->prev = prev;
            }

            buddy_node* left = cur < other ? cur : other;
            buddy_node* right = cur < other ? other : cur;

            left->order++;
            left->free = 1;

            right->order = 0;
            right->free = 0;

            cur = left;

            prev->next = cur;
            next->prev = cur;
            cur->next = next;
            cur->prev = prev;

            if (!alloc->head) {
                alloc->head = cur;
                cur->prev = NULL;
            }
        } else {
            break;
        }
    }
}