#include "../include/memutils.h"

#define X(name) #name,
const char* alloc_res_names[] = { ALLOC_RES_LIST(X) };
#undef X

_Thread_local alloc_res alloc_err = RES_SUCCESS;

const char* alloc_get_errmsg() {
    return alloc_res_names[alloc_err];
}

size_t log2_int(size_t x) {
    #if __SIZE_WIDTH__ == 64
    signed long long r = -1;
    #else
    signed long r = -1;
    #endif

    while (x) {
        x >>= 1;
        r++;
    }

    return r;
}

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;

    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }

    return dest;
}