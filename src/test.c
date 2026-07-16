#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <sys/mman.h>
#include "../include/alloc.h"
#include "impls/buddy_impl.h"

void dump(buddy* buddy) {
    buddy_impl* alloc = (buddy_impl*)buddy;

    buddy_node* cur = alloc->mem;
    char *end = (char *)alloc->mem + alloc->max_size;

    while ((char *)cur < end) {
        printf("offset=%4zu  size=%4zu  %s\n",
               (size_t)((char *)cur - (char *)alloc->mem),
               cur->order,
               cur->free ? "FREE" : "USED");

        if (cur->order == 0) {
            puts("ERROR: zero-sized block");
            break;
        }

        cur = (buddy_node *)((char *)cur + MAX(1ULL << cur->order, sizeof(buddy_node)));
    }
    printf("\n");

}

void test(allocator* alloc, const size_t size, const size_t fragments) {
    void** arr = malloc(sizeof(void*) * fragments);
    for (size_t i = 0; i < fragments; i++) {
        void* ptr = allocator_alloc(alloc, size);
        
        assert(ptr != NULL);

        arr[i] = ptr;
        
    }

    for (size_t i = 0; i < fragments; i++) {
        allocator_free(alloc, arr[i]);
    }

    free(arr);

    for (size_t i = 0; i < fragments; i++) {
        void* ptr = allocator_alloc(alloc, size * fragments);
        assert(ptr != NULL);
        allocator_free(alloc, ptr);
    }
}

void* map(size_t size) {
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void unmap(void* ptr, size_t size) {
    munmap(ptr, size);
}

int main() {
    alloc_backend backend = { .alloc = map, .free = unmap };

    buddy raw = buddy_create(&backend, 1024);
    allocator alloc = { .ctx = &raw, .destroy = (destroy_func)buddy_destroy, .alloc = (alloc_func)buddy_alloc, .free = (free_func)buddy_free };

    test(&alloc, 1, 1024);

    allocator_destroy(&alloc);
}