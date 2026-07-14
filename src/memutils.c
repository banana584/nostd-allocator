#include "../include/memutils.h"

#define X(name) #name,
const char* alloc_res_names[] = { ALLOC_RES_LIST(X) };
#undef X

_Thread_local alloc_res alloc_err = RES_SUCCESS;