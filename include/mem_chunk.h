#ifndef MEM_CHUNK_H
#define MEM_CHUNK_H

#include <stddef.h>

typedef struct mem_chunk {
	void* start;
	size_t size;
} MemChunk;

int are_chunks_adj(MemChunk* chunk1, MemChunk* chunk2);
MemChunk* get_left_chunk(MemChunk* chunk1, MemChunk* chunk2);
MemChunk* get_right_chunk(MemChunk* chunk1, MemChunk* chunk2);

#endif
