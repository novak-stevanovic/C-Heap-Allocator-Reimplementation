#ifndef MEM_CHUNK_LIST_H
#define MEM_CHUNK_LIST_H

#include <stddef.h>
#include "mem_chunk.h"

typedef struct mem_chunk_list {
	size_t current_size;
	MemChunk chunks[MAX_CHUNK_COUNT];
} MemChunkList;

int append_chunk(MemChunkList* chunk_list, MemChunk* new_chunk);
int insert_chunk(MemChunkList* chunk_list, MemChunk* new_chunk, int index);
int find_chunk_ind(MemChunkList* chunk_list, void* chunk_start);
int remove_chunk(MemChunkList* chunk_list, size_t removed_chunk_index);
void print_chunk_list(MemChunkList* chunk_list);
int find_chunk_spot_by_addr(MemChunkList* chunk_list, void* start_addr);

#endif
