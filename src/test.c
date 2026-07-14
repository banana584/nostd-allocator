#include <stdio.h>
#include <sys/mman.h>
#include "../include/buddy.h"

void* alloc_mem(size_t size) {
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void free_mem(void* ptr, size_t size) {
    munmap(ptr, size);
}

int main() {
    alloc_backend backend = { .alloc = alloc_mem, .free = free_mem };
    buddy* alloc = buddy_create(backend, 1024);

    int* p1 = buddy_alloc(alloc, sizeof(int));
    int* p2 = buddy_alloc(alloc, sizeof(int));

    *p1 = 2;
    *p2 = 36346;

    printf("%d %d\n", *p1, *p2);

    buddy_free(alloc, p1);
    buddy_free(alloc, p2);

    buddy_destroy(alloc);

    return 0;
}