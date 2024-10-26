#include <stdio.h>

#include "mem_chunk_list.h"
#include "misc.h"

void init_chunk_list(MemChunkList* chunk_list, size_t cap, MemChunk* chunk_array) {
    chunk_list->size = 0;
    chunk_list->cap = cap;
    chunk_list->chunks = chunk_array;
}

int append_chunk(MemChunkList* chunk_list, MemChunk* new_chunk) {
    if((new_chunk == NULL) || (chunk_list->size >= chunk_list->cap)) return -1;

    MemChunk *chunks = chunk_list->chunks;

    chunks[chunk_list->size] = *new_chunk;

    chunk_list->size++;

    return 0;
}

int left_shift_all_after(MemChunkList* chunk_list, size_t start) {
    if((start > chunk_list->size - 1) || (chunk_list->size < 1)) return -1;

    MemChunk *chunks = chunk_list->chunks;
    size_t current_size = chunk_list->size;
    size_t i;
    for(i = start; i < current_size - 1; i++)
        chunks[i] = chunks[i + 1];

    return 0;
}

int right_shift_all_from(MemChunkList* chunk_list, size_t start) {
    if((start >= chunk_list->size) || (chunk_list->size >= chunk_list->cap)) return -1;

    size_t i;
    MemChunk *chunks = chunk_list->chunks;
    for(i = chunk_list->size; i > start; i--)
        chunks[i] = chunks[i - 1];

    return 0;
}

#define BSEARCH_ERR -2

int bsearch_prev_ind(MemChunkList* chunk_list, void* chunk_start) {
    if(chunk_start == NULL) return BSEARCH_ERR;

    size_t current_size = chunk_list->size;
    if(current_size == chunk_list->cap) return BSEARCH_ERR;

    MemChunk *chunks = chunk_list->chunks;

    int start = 0;
    int end = current_size - 1;
    int mid;
    while(start <= end) {
        mid = start + (end - start) / 2;
        if(chunk_start == chunks[mid].start) return (mid - 1);
        if(chunk_start > chunks[mid].start) start = mid + 1;
        else end = mid - 1;
    }

    return (start < end ? start : end);

}

int find_chunk_ind(MemChunkList* chunk_list, void* chunk_start) {
    if(chunk_start == NULL) return -1;

    int bsearch_status = bsearch_prev_ind(chunk_list, chunk_start);
    ASSERT(bsearch_status != BSEARCH_ERR, "Binary search error.");
    int potential_ind = bsearch_status + 1;
    ASSERT(potential_ind >= 0, "Potential index must be non-negative.");
    ASSERT((size_t)potential_ind < chunk_list->size, "Potential index value must be less than size of chunk list. ");
    return ((chunk_list->chunks[potential_ind].start == chunk_start) ? potential_ind : -1);
}

int remove_chunk(MemChunkList* chunk_list, size_t removed_chunk_index) {
    ASSERT(removed_chunk_index < chunk_list->size, "Failed removing chunk: invalid index.");

    if(left_shift_all_after(chunk_list, removed_chunk_index) == 0) {
        chunk_list->size--;
        return 0;
    }
    else return -1;
}

int insert_chunk_by_addr(MemChunkList* chunk_list, MemChunk* new_chunk) {
    if(new_chunk == NULL) return -1;

    size_t current_size = chunk_list->size;
    if(current_size == chunk_list->cap) return -1;

    int bsearch_status = bsearch_prev_ind(chunk_list, new_chunk->start);
    ASSERT(bsearch_status != BSEARCH_ERR, "Binary search error.");
    int ind = bsearch_status + 1;
    ASSERT(ind >= 0, "Potential index must be non-negative.");

    if((current_size != 0 && ((size_t)ind < chunk_list->size))) {
        int shift_status = right_shift_all_from(chunk_list, ind);
        ASSERT(shift_status == 0, "Shift status must be 0.");
    }

    chunk_list->chunks[ind] = *new_chunk;
    chunk_list->size++;

    return ind;
}

void print_chunk_list(MemChunkList* chunk_list) {

    if(chunk_list->size == 0) return;

    size_t i;
    for(i = 0; i < chunk_list->size - 1; i++) {
        printf("size: %ld, start: %p, end: %p | ", chunk_list->chunks[i].size, chunk_list->chunks[i].start, chunk_list->chunks[i].start + chunk_list->chunks[i].size);
    }    
    printf("size: %ld, start: %p, end: %p", chunk_list->chunks[i].size, chunk_list->chunks[i].start, chunk_list->chunks[i].start + chunk_list->chunks[i].size);

    putchar('\n');
}
