#ifndef NOSTDALLOC_INCLUDE_ARENA_H
#define NOSTDALLOC_INCLUDE_ARENA_H

#include <stddef.h>
#include "memutils.h"

typedef struct arena arena;

arena* arena_create(alloc_backend backend, const size_t cap);

void arena_destroy(arena* arena);

void* arena_alloc(arena* arena, const size_t size);

void arena_reset(arena* arena);

#endif