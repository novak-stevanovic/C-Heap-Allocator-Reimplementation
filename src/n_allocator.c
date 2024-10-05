#include <stdio.h>
#include "n_allocator.h"

char heap[HEAP_CAPACITY] = {0};
MemChunkList allocated_chunks = {0};
MemChunkList free_chunks = {0};

void init() {
	MemChunk free_heap = {
		.start = heap,
		.size = HEAP_CAPACITY
	};
	append_mem_chunk(&free_chunks, &free_heap);
}

int find_spot(size_t size) {
	size_t i;
	for(i = 0; i < free_chunks.current_size; i++) {
		//printf("Current free chunk size: %ld, needed size: %ld\n", free_chunks.chunks[i].size, size);
		if(size <= free_chunks.chunks[i].size) return i;
	}

	return -1;
	
}

int shrinkOrRemoveFreeChunk(int free_chunk_index, size_t shrinkage) {
	MemChunk* free_chunk = &free_chunks.chunks[free_chunk_index];

	if(free_chunk->size == shrinkage) {
		remove_mem_chunk(&free_chunks, free_chunk_index);
		return 0;
	}
	else if(free_chunk->size > shrinkage) {
		free_chunk->size -= shrinkage;
		//printf("%p updated to %p\n", free_chunk->start, free_chunk->start + shrinkage);
		free_chunk->start += shrinkage;
		return 0;
	}
	else return -1;
}

void* n_allocate(size_t size) {
	if(size == 0) return NULL;

	int free_spot_index = find_spot(size);
	if(free_spot_index == -1) return NULL;
	//printf("1: %d\n", free_spot_index);

	MemChunk* free_spot = &free_chunks.chunks[free_spot_index];

	void* mem_chunk_start = free_spot->start;
	int shrank_chunk = shrinkOrRemoveFreeChunk(free_spot_index, size);
	if(shrank_chunk == -1) return NULL;
	//printf("2: %d\n", shrank_chunk);

	int spot_in_allocated_list = find_mem_chunk_spot_by_addr(&allocated_chunks, mem_chunk_start);
	if(spot_in_allocated_list == -1) return NULL;
	//printf("3: %d\n", spot_in_allocated_list);

	MemChunk new = {
		.start = mem_chunk_start,
		.size = size
	};

	//printf("New start: %p, size: %ld, end: %p\n", new.start, new.size, new.start + new.size);
	//printf("ALLOCATED CHUNKS:\n");
	//print_mem_chunk_list(&allocated_chunks);
	//printf("FREE CHUNKS:\n");
	//print_mem_chunk_list(&free_chunks);
	//printf("HEAP starts at: %p, ends at: %p\n", heap, heap + HEAP_CAPACITY);

	return (insert_mem_chunk(&allocated_chunks, &new, spot_in_allocated_list) == 0 ? mem_chunk_start : NULL);
}

int merge_free_mem_chunks(size_t left_mem_chunk_index, size_t right_mem_chunk_index) {
	free_chunks.chunks[left_mem_chunk_index].size += free_chunks.chunks[right_mem_chunk_index].size;
	remove_mem_chunk(&free_chunks, right_mem_chunk_index);
	return 0;
}


int join_adjacent_free_chunks(size_t center_mem_chunk_index) {
	if(center_mem_chunk_index >= free_chunks.current_size) return -1;

	MemChunk* center_mem_chunk = &free_chunks.chunks[center_mem_chunk_index];

	if(center_mem_chunk_index > 0) {
		size_t left_mem_chunk_index = center_mem_chunk_index - 1;
		MemChunk* left_mem_chunk = &free_chunks.chunks[left_mem_chunk_index];
		if(are_mem_chunks_adjacent(left_mem_chunk, center_mem_chunk) == 1) {
			merge_free_mem_chunks(left_mem_chunk_index, center_mem_chunk_index);
			center_mem_chunk_index = left_mem_chunk_index;
		}
	}

	center_mem_chunk = &free_chunks.chunks[center_mem_chunk_index];

	if(center_mem_chunk_index < free_chunks.current_size) {
		size_t right_mem_chunk_index = center_mem_chunk_index + 1;
		MemChunk* right_mem_chunk = &free_chunks.chunks[right_mem_chunk_index];
		if(are_mem_chunks_adjacent(center_mem_chunk, right_mem_chunk) == 1)
			merge_free_mem_chunks(center_mem_chunk_index, right_mem_chunk_index);
	}

	return 0;
}

int n_free(void* ptr) {
	if(ptr == NULL) return -1;

	//printf("1\n");
	int chunk_for_removal_index = find_mem_chunk_index(&allocated_chunks, ptr);
	if(chunk_for_removal_index == -1) return -1;
	MemChunk* chunk_for_removal = &allocated_chunks.chunks[chunk_for_removal_index];

	//printf("2\n");
	int spot_in_free_chunks = find_mem_chunk_spot_by_addr(&free_chunks, ptr);
	if(spot_in_free_chunks == -1) return -1;

	//printf("3\n");
	int inserted_into_free = insert_mem_chunk(&free_chunks, chunk_for_removal, spot_in_free_chunks);
	if(inserted_into_free == -1) return -1;

	//printf("4\n");
	int removed = remove_mem_chunk(&allocated_chunks, chunk_for_removal_index);
	if(removed == -1) return -1;

	//printf("5\n");
	int joined = join_adjacent_free_chunks(spot_in_free_chunks);

	return 0;

}

void mark(char* start, char* end, char c) {
	//printf("in mark\n");
	char* it;
	int i = 0;
	for(it = start; it < end; it++) {
		//printf("%d%c ", i, c);
		if(((*it) == TAKEN_IND) || ((*it) == FREE_IND)) {
			//printf("1\n");
			*it = ERR_IND;
		}
		else {
			//printf("2\n");
			*it = c;
		}

		i++;
	}
}

void print_heap() {
	size_t i;
	mark(heap, heap + HEAP_CAPACITY, UNKN_IND);

	for(i = 0; i < allocated_chunks.current_size; i++) {
		MemChunk* curr_chunk = &(allocated_chunks.chunks[i]);
		//printf("allocating: %ld %p\n", curr_chunk->size, curr_chunk->start);
		mark(curr_chunk->start, curr_chunk->start + curr_chunk->size, TAKEN_IND);
	}

	for(i = 0; i < free_chunks.current_size; i++) {
		MemChunk* curr_chunk = &(free_chunks.chunks[i]);
		//printf("freeing: %ld %p\n", curr_chunk->size, curr_chunk->start);
		mark(curr_chunk->start, curr_chunk->start + curr_chunk->size, FREE_IND);
	}

	for(i = 0; i < HEAP_CAPACITY; i++) {
		printf("%c ", heap[i]);
	}
	putchar('\n');
}

