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

int append_chunk(MemChunkList* chunk_list, MemChunk* new_chunk);
int insert_chunk(MemChunkList* chunk_list, MemChunk* new_chunk, int index);
int find_chunk_ind(MemChunkList* chunk_list, void* chunk_start);
int remove_chunk(MemChunkList* chunk_list, size_t removed_chunk_index);
void print_chunk_list(MemChunkList* chunk_list);
int find_chunk_spot_by_addr(MemChunkList* chunk_list, void* start_addr);

int are_chunks_adj(MemChunk* chunk1, MemChunk* chunk2);
MemChunk* get_left_chunk(MemChunk* chunk1, MemChunk* chunk2);
MemChunk* get_right_chunk(MemChunk* chunk1, MemChunk* chunk2);
MemChunk* get_merged_chunk(MemChunk* chunk1, MemChunk* chunk2);

#endif
