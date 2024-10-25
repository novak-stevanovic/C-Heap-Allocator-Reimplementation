#include <stdio.h>

#include "n_allocator.h"
#include "misc.h"
#include "n_reallocator.h"
#include "n_allocator_internal.h"
#include "heap.h"

#define MAX_ALLOCED_CHUNKS 50
#define MAX_FREE_CHUNKS 50

#define ENABLE_DEBUG 1
#define DEBUG_ALLOC(x) if(ENABLE_DEBUG) printf("Allocating size: %ld\n", x);
#define DEBUG_FREE(x) if(ENABLE_DEBUG) printf("Freeing size: %ld\n", x);
#define DEBUG_NREALLOC(x) if(ENABLE_DEBUG) printf("Reallocating size: %ld\n", x);

MemChunk alloced_chunks_array[MAX_ALLOCED_CHUNKS];
MemChunk free_chunks_array[MAX_FREE_CHUNKS];

MemChunkList alloced_chunks;
MemChunkList free_chunks;

void nalloc_init() {
    init_chunk_list(&alloced_chunks, MAX_ALLOCED_CHUNKS, alloced_chunks_array);
    init_chunk_list(&free_chunks, MAX_FREE_CHUNKS, free_chunks_array);

    MemChunk free_heap = {
        .start = heap,
        .size = HEAP_CAPACITY
    };

    append_chunk(&free_chunks, &free_heap);
}

// First-fit algorithm
int find_free_space(size_t size) {
    size_t i;
    for(i = 0; i < free_chunks.size; i++) {
        if(size <= free_chunks.chunks[i].size) return i;
    }

    return -1;
}

void* nalloc(size_t size) {
    DEBUG_ALLOC(size);
    if(size == 0) return NULL;

    if(alloced_chunks.size >= alloced_chunks.cap) return NULL;

    int free_spot_ind = find_free_space(size);
    if(free_spot_ind == -1) return NULL;

    MemChunk* free_spot = &free_chunks.chunks[free_spot_ind];

    void* chunk_start = free_spot->start;
    shrink_chunk(free_spot_ind, &free_chunks, size);

    MemChunk new = {
        .start = chunk_start,
        .size = size
    };

    int insert_status = insert_chunk_by_addr(&alloced_chunks, &new);
    ASSERT(insert_status >= 0, "Insert chunk returned non 0 status.");

    return chunk_start;
}

int nfree(void* ptr) {
    if(ptr == NULL) return -1;
    if(free_chunks.size >= free_chunks.cap) return -1;

    int removed_chunk_ind = find_chunk_ind(&alloced_chunks, ptr);
    if(removed_chunk_ind == -1) return -1;
    DEBUG_FREE(alloced_chunks.chunks[removed_chunk_ind].size);

    nfree_by_ind(removed_chunk_ind);

    return 0;
}

void* nrealloc(void* ptr, size_t new_size) {
    DEBUG_NREALLOC(new_size);
    return nreallocator_nrealloc(ptr, new_size);
}


