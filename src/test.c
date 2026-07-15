#include <stdio.h>
#include <sys/mman.h>
#include "../include/buddy.h"
#include "../include/slab.h"

void* alloc_mem(size_t size) {
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void free_mem(void* ptr, size_t size) {
    munmap(ptr, size);
}

int main() {
    alloc_backend backend = { .alloc = alloc_mem, .free = free_mem };
    buddy alloc = buddy_create(&backend, 1024);

    int* p1 = buddy_alloc(&alloc, sizeof(int));
    int* p2 = buddy_alloc(&alloc, sizeof(int));

    *p1 = 2;
    *p2 = 36346;

    buddy_free(&alloc, p1);
    buddy_free(&alloc, p2);

    buddy_destroy(&alloc);

    slab slab = slab_create(&backend, 3, 5);

    int* arr = slab_alloc(&slab, sizeof(int) * 5);

    for (size_t i = 0; i < 5; i++) {
        arr[i] = i * 2;
    }

    for (size_t i = 0; i < 5; i++) {
        printf("%ld %d\n", i, arr[i]);
    }

    slab_free(&slab, arr);

    slab_destroy(&slab);

    pool pool = pool_create(&backend, sizeof(float), 4);

    float* arr2[3];

    for (size_t i = 0; i < 3; i++) {
        arr2[i] = pool_alloc(&pool);
        *arr2[i] = 3.14159f * i;
    }

    for (size_t i = 0; i < 3; i++) {
        printf("%ld %f\n", i, *arr2[i]);
        pool_free(&pool, arr2[i]);
    }

    pool_destroy(&pool);

    arena arena = arena_create(&backend, sizeof(double) * 2);
    double* d1 = arena_alloc(&arena, sizeof(double));
    double* d2 = arena_alloc(&arena, sizeof(double));

    *d1 = 1155.21313;
    *d2 = 1231.12512;

    printf("%f %f\n", *d1, *d2);

    arena_destroy(&arena);

    return 0;
}