#include "mem_chunk.h"
#include "stdio.h"

int append_mem_chunk(MemChunkList* mem_chunk_list, MemChunk* new_mem_chunk) {
	if((new_mem_chunk == NULL) || (mem_chunk_list->current_size >= MAX_CHUNK_COUNT)) return -1;

	MemChunk *mem_chunks = mem_chunk_list->chunks;
	size_t current_size = mem_chunk_list->current_size;

	mem_chunks[current_size] = *new_mem_chunk;

	mem_chunk_list->current_size++;

	return 0;
}

int left_shift_all_after(MemChunkList* mem_chunk_list, size_t start) {
	if((start > mem_chunk_list->current_size - 1) || (mem_chunk_list->current_size < 1)) return -1;

	MemChunk *mem_chunks = mem_chunk_list->chunks;
	size_t current_size = mem_chunk_list->current_size;
	size_t i;
	for(i = start; i < current_size - 1; i++)
		mem_chunks[i] = mem_chunks[i + 1];

	mem_chunk_list->current_size--;

	return 0;
}

int right_shift_all_from(MemChunkList* mem_chunk_list, size_t start) {
	if((start >= mem_chunk_list->current_size) || (mem_chunk_list->current_size >= MAX_CHUNK_COUNT)) return -1;
	size_t i;
	MemChunk *mem_chunks = mem_chunk_list->chunks;
	for(i = mem_chunk_list->current_size; i > start; i--)
		mem_chunks[i] = mem_chunks[i - 1];

	mem_chunk_list->current_size++;

	return 0;
}

int find_mem_chunk_index(MemChunkList* mem_chunk_list, void* mem_chunk_start) {
	if(mem_chunk_start == NULL) return -1;

	MemChunk *mem_chunks = mem_chunk_list->chunks;
	size_t current_size = mem_chunk_list->current_size;
	size_t i;
	for(i = 0; i < current_size; i++) {
		if(mem_chunks[i].start == mem_chunk_start)
			return i;
	}

	return -1;
}

int remove_mem_chunk(MemChunkList* mem_chunk_list, size_t removed_mem_chunk_index) {
	return left_shift_all_after(mem_chunk_list, removed_mem_chunk_index);
}

int insert_mem_chunk(MemChunkList* mem_chunk_list, MemChunk* new_mem_chunk, int index) {
	int current_size = mem_chunk_list->current_size;

	if(index > current_size) return -1;
	else if(index == current_size) {
		append_mem_chunk(mem_chunk_list, new_mem_chunk);
		return 0;
	}
	else {
		if(right_shift_all_from(mem_chunk_list, index) == 0) {
			mem_chunk_list->chunks[index] = *new_mem_chunk;
			return 0;
		}
		else return -1;
	}

}

int find_mem_chunk_spot_by_addr(MemChunkList* mem_chunk_list, void* start_addr) {
	if(mem_chunk_list->current_size >= MAX_CHUNK_COUNT) return -1;
	if(mem_chunk_list->current_size == 0) return 0;

	size_t i;
	for(i = 0; i < mem_chunk_list->current_size; i++) {
		//printf("find i: %ld, curr_size: %ld\n", i, mem_chunk_list->current_size);
		if(mem_chunk_list->chunks[i].start > start_addr) return i;
	}

	return mem_chunk_list->current_size;
}


int are_mem_chunks_adjacent(MemChunk* mem_chunk1, MemChunk* mem_chunk2) {
	MemChunk* left_mem_chunk = get_left_mem_chunk(mem_chunk1, mem_chunk2);
	MemChunk* right_mem_chunk = get_right_mem_chunk(mem_chunk1, mem_chunk2);

	return ((left_mem_chunk->start + left_mem_chunk->size) == right_mem_chunk->start);
}

MemChunk* get_left_mem_chunk(MemChunk* mem_chunk1, MemChunk* mem_chunk2) {
	if(mem_chunk1->start == mem_chunk2->start) return NULL;

	return (mem_chunk1->start < mem_chunk2->start ? mem_chunk1 : mem_chunk2);
	
}

MemChunk* get_right_mem_chunk(MemChunk* mem_chunk1, MemChunk* mem_chunk2) {
	if(mem_chunk1->start == mem_chunk2->start) return NULL;

	return (mem_chunk1->start > mem_chunk2->start ? mem_chunk1 : mem_chunk2);
}

void print_mem_chunk_list(MemChunkList* mem_chunk_list) {
	size_t i;
	for(i = 0; i < mem_chunk_list->current_size; i++) {
		printf("Chunk: %ld, starting at: %p, size: %ld, ending at: %p.\n", i, mem_chunk_list->chunks[i].start, mem_chunk_list->chunks[i].size, mem_chunk_list->chunks[i].start + 
		mem_chunk_list->chunks[i].size);
	}
}

