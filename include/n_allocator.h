#ifndef N_ALLOCATOR_H
#define N_ALLOCATOR_H

#include "heap.h"
#include "mem_chunk.h"
#include "mem_chunk_list.h"

extern MemChunkList alloced_chunks;
extern MemChunkList free_chunks;

void nalloc_init();
void* n_alloc(size_t size);
int n_free(void* ptr);

#endif
