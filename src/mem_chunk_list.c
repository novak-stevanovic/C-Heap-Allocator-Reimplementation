#include <stdio.h>
#include "mem_chunk_list.h"
#include "misc.h"

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

int find_chunk_ind(MemChunkList* chunk_list, void* chunk_start) {
    if(chunk_start == NULL) return -1;

    MemChunk *chunks = chunk_list->chunks;
    size_t current_size = chunk_list->size;
    size_t i;
    for(i = 0; i < current_size; i++) {
        if(chunks[i].start == chunk_start) return i;
    }

    return -1;
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

    size_t i = 0;
    for(; i < current_size; i++) {
        if(chunk_list->chunks[i].start > new_chunk->start) break;
    }

    if(current_size > 0) right_shift_all_from(chunk_list, i);
    chunk_list->chunks[i] = *new_chunk;

    chunk_list->size++;

    return i;
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
