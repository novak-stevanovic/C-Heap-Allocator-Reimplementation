#ifndef MEM_CHUNK_LIST_H
#define MEM_CHUNK_LIST_H

#include <stddef.h>
#include "mem_chunk.h"

#define MAX_CHUNK_COUNT 50

typedef struct mem_chunk_list {
	size_t current_size;
	MemChunk chunks[MAX_CHUNK_COUNT];
} MemChunkList;

int append_chunk(MemChunkList* chunk_list, MemChunk* new_chunk);
int insert_chunk_by_addr(MemChunkList* chunk_list, MemChunk* new_chunk); //?
int find_chunk_ind(MemChunkList* chunk_list, void* chunk_start);
int remove_chunk(MemChunkList* chunk_list, size_t removed_chunk_index);
void print_chunk_list(MemChunkList* chunk_list);

#endif
