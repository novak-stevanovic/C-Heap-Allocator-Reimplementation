#include "mem_chunk_list.h"

int append_chunk(MemChunkList* chunk_list, MemChunk* new_chunk) {
	if((new_chunk == NULL) || (chunk_list->current_size >= MAX_CHUNK_COUNT)) return -1;

	MemChunk *chunks = chunk_list->chunks;
	size_t current_size = chunk_list->current_size;

	chunks[current_size] = *new_chunk;

	chunk_list->current_size++;

	return 0;
}

int left_shift_all_after(MemChunkList* chunk_list, size_t start) {
	if((start > chunk_list->current_size - 1) || (chunk_list->current_size < 1)) return -1;

	MemChunk *chunks = chunk_list->chunks;
	size_t current_size = chunk_list->current_size;
	size_t i;
	for(i = start; i < current_size - 1; i++)
		chunks[i] = chunks[i + 1];

	chunk_list->current_size--;

	return 0;
}

int right_shift_all_from(MemChunkList* chunk_list, size_t start) {
	if((start >= chunk_list->current_size) || (chunk_list->current_size >= MAX_CHUNK_COUNT)) return -1;
	size_t i;
	MemChunk *chunks = chunk_list->chunks;
	for(i = chunk_list->current_size; i > start; i--)
		chunks[i] = chunks[i - 1];

	chunk_list->current_size++;

	return 0;
}

int find_chunk_ind(MemChunkList* chunk_list, void* chunk_start) {
	if(chunk_start == NULL) return -1;

	MemChunk *chunks = chunk_list->chunks;
	size_t current_size = chunk_list->current_size;
	size_t i;
	for(i = 0; i < current_size; i++) {
		if(chunks[i].start == chunk_start)
			return i;
	}

	return -1;
}

int remove_chunk(MemChunkList* chunk_list, size_t removed_chunk_index) {
	return left_shift_all_after(chunk_list, removed_chunk_index);
}

int insert_chunk(MemChunkList* chunk_list, MemChunk* new_chunk, int index) {
	int current_size = chunk_list->current_size;

	if(index > current_size) return -1;
	else if(index == current_size) {
		append_chunk(chunk_list, new_chunk);
		return 0;
	}
	else {
		if(right_shift_all_from(chunk_list, index) == 0) {
			chunk_list->chunks[index] = *new_chunk;
			return 0;
		}
		else return -1;
	}

}

int find_chunk_spot_by_addr(MemChunkList* chunk_list, void* start_addr) {
	if(chunk_list->current_size >= MAX_CHUNK_COUNT) return -1;
	if(chunk_list->current_size == 0) return 0;

	size_t i;
	for(i = 0; i < chunk_list->current_size; i++) {
		if(chunk_list->chunks[i].start > start_addr) return i;
	}

	return chunk_list->current_size;
}

void print_chunk_list(MemChunkList* chunk_list) {
	size_t i;
	for(i = 0; i < chunk_list->current_size; i++) {
		printf("Chunk: %ld, starting at: %p, size: %ld, ending at: %p.\n", i, chunk_list->chunks[i].start, chunk_list->chunks[i].size, chunk_list->chunks[i].start + 
		chunk_list->chunks[i].size);
	}
}
