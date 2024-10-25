#ifndef N_ALLOCATOR_H
#define N_ALLOCATOR_H

#include "mem_chunk.h"

void nalloc_init();
void* nalloc(size_t size);
int nfree(void* ptr);
void* nrealloc(void* ptr, size_t new_size);

#endif
