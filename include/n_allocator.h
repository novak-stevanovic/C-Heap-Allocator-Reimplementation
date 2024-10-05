#ifndef N_ALLOCATOR_H
#define N_ALLOCATOR_H

#include "mem_chunk.h"

#define HEAP_CAPACITY 80
#define FREE_IND 'o'
#define TAKEN_IND 'x'
#define UNKN_IND 'U'
#define ERR_IND 'U'

extern char heap[HEAP_CAPACITY];
extern MemChunkList allocated_chunks;
extern MemChunkList free_chunks;

void init();
void* n_allocate(size_t size);
int n_free(void* ptr);
int join_adjacent_free_chunks(size_t index);
void print_heap();

#endif
