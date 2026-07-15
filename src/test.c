#include <stdio.h>
#include <sys/mman.h>
#include "../include/buddy.h"
#include "../include/slab.h"
#include "../include/alloc.h"

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

    slab aSlab = slab_create(&backend, 3, 5);

    int* arr = slab_alloc(&aSlab, sizeof(int) * 5);

    for (size_t i = 0; i < 5; i++) {
        arr[i] = i * 2;
    }

    for (size_t i = 0; i < 5; i++) {
        printf("%ld %d\n", i, arr[i]);
    }

    arr = slab_realloc(&aSlab, arr, sizeof(int) * 6);

    arr[5] = 5 * 2;

    printf("%d %d\n", 5, arr[5]);

    slab_free(&aSlab, arr);

    slab_destroy(&aSlab);

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

    slab new = slab_create(&backend, 4, 5);
    allocator gen = { .ctx = &new, .destroy = (destroy_func)slab_destroy, .alloc = (alloc_func)slab_alloc, .realloc = (realloc_func)slab_realloc, .free = (free_func)slab_free };

    int* g1 = allocator_alloc(&gen, sizeof(int));
    int* g2 = allocator_alloc(&gen, sizeof(int));

    *g1 = 215;
    *g2 = 1516;

    printf("%d %d\n", *g1, *g2);

    allocator_free(&gen, g1);
    allocator_free(&gen, g2);

    allocator_destroy(&gen);

    return 0;
}