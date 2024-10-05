#ifndef MEM_CHUNK_H
#define MEM_CHUNK_H

#include <stddef.h>

#define MAX_CHUNK_COUNT 50

typedef struct mem_chunk {
	void* start;
	size_t size;
} MemChunk;

typedef struct mem_chunk_list {
	size_t current_size;
	MemChunk chunks[MAX_CHUNK_COUNT];
} MemChunkList;

int append_mem_chunk(MemChunkList* mem_chunk_list, MemChunk* new_mem_chunk);
int insert_mem_chunk(MemChunkList* mem_chunk_list, MemChunk* new_mem_chunk, int index);
int find_mem_chunk_index(MemChunkList* mem_chunk_list, void* mem_chunk_start);
int remove_mem_chunk(MemChunkList* mem_chunk_list, size_t removed_mem_chunk_index);
void print_mem_chunk_list(MemChunkList* mem_chunk_list);
int find_mem_chunk_spot_by_addr(MemChunkList* mem_chunk_list, void* start_addr);

int are_mem_chunks_adjacent(MemChunk* mem_chunk1, MemChunk* mem_chunk2);
MemChunk* get_left_mem_chunk(MemChunk* mem_chunk1, MemChunk* mem_chunk2);
MemChunk* get_right_mem_chunk(MemChunk* mem_chunk1, MemChunk* mem_chunk2);
MemChunk* get_merged_mem_chunk(MemChunk* mem_chunk1, MemChunk* mem_chunk2);

#endif
