#ifndef N_ALLOCATOR_H
#define N_ALLOCATOR_H

#include "heap.h"
#include "mem_chunk.h"
#include "mem_chunk_list.h"

#define VISUALIZE_HEAP_MAX 100
#define FREE_IND 'o'
#define ALLOCED_IND 'x'
#define ERR_IND 'U'

extern MemChunkList alloced_chunks;
extern MemChunkList free_chunks;

void nalloc_init();
void* nalloc(size_t size);
int nfree(void* ptr);

void print_chunks();
void visualize_heap();

#endif
