#include <stdio.h>
#include "n_allocator.h"

char heap[HEAP_CAPACITY] = {0};
MemChunkList alloced_chunks = {0};
MemChunkList free_chunks = {0};

void init() {
	MemChunk free_heap = {
		.start = heap,
		.size = HEAP_CAPACITY
	};
	append_chunk(&free_chunks, &free_heap);
}

int find_free_space(size_t size) {
	size_t i;
	for(i = 0; i < free_chunks.current_size; i++) {
		if(size <= free_chunks.chunks[i].size) return i;
	}

	return -1;
}

int shrink_free_chunk(size_t free_chunk_ind, size_t size) {
	MemChunk* free_chunk = &free_chunks.chunks[free_chunk_ind];

	if(free_chunk->size == size) {
		remove_chunk(&free_chunks, free_chunk_ind);
		return 0;
	}
	else if(free_chunk->size > size) {
		free_chunk->size -= size;
		free_chunk->start += size;
		return 0;
	}
	else return -1;
}

void* n_alloc(size_t size) {
	if(size == 0) return NULL;

	int free_spot_ind = find_free_space(size);
	if(free_spot_ind == -1) return NULL;

	MemChunk* free_spot = &free_chunks.chunks[free_spot_ind];

	void* chunk_start = free_spot->start;
	int shrank_status = shrink_free_chunk(free_spot_ind, size);
	if(shrank_status == -1) return NULL;

	int spot_in_alloced_list = find_chunk_spot_by_addr(&alloced_chunks, chunk_start);
	if(spot_in_alloced_list == -1) return NULL;

	MemChunk new = {
		.start = chunk_start,
		.size = size
	};

	return (insert_chunk(&alloced_chunks, &new, spot_in_alloced_list) == 0 ? chunk_start : NULL);
}

int merge_free_chunks(size_t left_chunk_ind, size_t right_chunk_ind) {
	free_chunks.chunks[left_chunk_ind].size += free_chunks.chunks[right_chunk_ind].size;
	remove_chunk(&free_chunks, right_chunk_ind);
	return 0;
}


int join_adj_free_chunks(size_t center_chunk_ind) {
	if(center_chunk_ind >= free_chunks.current_size) return -1;

	MemChunk* center_chunk = &free_chunks.chunks[center_chunk_ind];

	if(center_chunk_ind > 0) {
		size_t left_chunk_ind = center_chunk_ind - 1;
		MemChunk* left_chunk = &free_chunks.chunks[left_chunk_ind];

		if(are_chunks_adj(left_chunk, center_chunk) == 1) {
			merge_free_chunks(left_chunk_ind, center_chunk_ind);
			center_chunk_ind = left_chunk_ind;
		}
	}

	center_chunk = &free_chunks.chunks[center_chunk_ind];

	if(center_chunk_ind < free_chunks.current_size) {
		size_t right_chunk_ind = center_chunk_ind + 1;
		MemChunk* right_chunk = &free_chunks.chunks[right_chunk_ind];

		if(are_chunks_adj(center_chunk, right_chunk) == 1)
			merge_free_chunks(center_chunk_ind, right_chunk_ind);
	}

	return 0;
}

int n_free(void* ptr) {
	if(ptr == NULL) return -1;

	int removed_chunk_ind = find_chunk_ind(&alloced_chunks, ptr);
	if(removed_chunk_ind == -1) return -1;
	MemChunk* removed_chunk = &alloced_chunks.chunks[removed_chunk_ind];

	int ind_in_free_chunks = find_chunk_spot_by_addr(&free_chunks, ptr);
	if(ind_in_free_chunks == -1) return -1;

	int inserted_status = insert_chunk(&free_chunks, removed_chunk, ind_in_free_chunks);
	if(inserted_status == -1) return -1;

	int removed_status = remove_chunk(&alloced_chunks, removed_chunk_ind);
	if(removed_status == -1) return -1;

	int joined = join_adj_free_chunks(ind_in_free_chunks);

	return 0;

}

void mark(char* start, char* end, char c) {
	char* it;
	int i = 0;
	for(it = start; it < end; it++) {
		if((*it) != UNKN_IND)
			*it = ERR_IND;
		else
			*it = c;

		i++;
	}
}

void print_heap() {
	size_t i;
	mark(heap, heap + HEAP_CAPACITY, UNKN_IND);

	for(i = 0; i < alloced_chunks.current_size; i++) {
		MemChunk* curr_chunk = &(alloced_chunks.chunks[i]);
		mark(curr_chunk->start, curr_chunk->start + curr_chunk->size, TAKEN_IND);
	}

	for(i = 0; i < free_chunks.current_size; i++) {
		MemChunk* curr_chunk = &(free_chunks.chunks[i]);
		mark(curr_chunk->start, curr_chunk->start + curr_chunk->size, FREE_IND);
	}

	for(i = 0; i < HEAP_CAPACITY; i++) {
        printf("%c ", heap[i]);
	}
	putchar('\n');
}

