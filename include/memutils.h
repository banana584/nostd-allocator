#ifndef NOSTDALLOC_INCLUDE_MEMUTILS_H
#define NOSTDALLOC_INCLUDE_MEMUTILS_H

#include <stddef.h>

typedef struct alloc_backend {
    void* (*alloc)(size_t size);
    void (*free)(void* ptr, size_t size);
} alloc_backend;

#define ALLOC_RES_LIST(X) \
    X(SUCCESS) \
    X(INVALID_ARG) \
    X(NO_MEM)

typedef enum alloc_res {
    #define X(name) RES_##name,
    ALLOC_RES_LIST(X)
    #undef X
} alloc_res;

extern const char* alloc_res_names[];

extern _Thread_local alloc_res alloc_err;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif