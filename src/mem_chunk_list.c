#include <stdio.h>
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

    return 0;
}

int right_shift_all_from(MemChunkList* chunk_list, size_t start) {
    if((start >= chunk_list->current_size) || (chunk_list->current_size >= MAX_CHUNK_COUNT)) return -1;
    size_t i;
    MemChunk *chunks = chunk_list->chunks;
    for(i = chunk_list->current_size; i > start; i--)
        chunks[i] = chunks[i - 1];

    return 0;
}

int find_chunk_ind(MemChunkList* chunk_list, void* chunk_start) {
    printf("0a");
    if(chunk_start == NULL) return -1;

    MemChunk *chunks = chunk_list->chunks;
    size_t current_size = chunk_list->current_size;
    size_t i;
    printf("1a");
    for(i = 0; i < current_size; i++) {
        printf("%p == %p ? \n", chunk_start, chunks[i].start);
        if(chunks[i].start == chunk_start) return i;
    }
    printf("2a");

    return -1;
}

int remove_chunk(MemChunkList* chunk_list, size_t removed_chunk_index) {
    if(left_shift_all_after(chunk_list, removed_chunk_index) == 0) {
        chunk_list->current_size--;
        return 0;
    }
    else return -1;
}

int insert_chunk_by_addr(MemChunkList* chunk_list, MemChunk* new_chunk) {
    if(new_chunk == NULL) return -1;

    size_t current_size = chunk_list->current_size;
    if(current_size == MAX_CHUNK_COUNT) return -1;

    size_t i = 0;
    for(; i < current_size; i++) {
        if(chunk_list->chunks[i].start > new_chunk->start)

            break;
    }

    if(current_size > 0) right_shift_all_from(chunk_list, i);
    chunk_list->chunks[i] = *new_chunk;

    chunk_list->current_size++;

    return i;
}

void print_chunk_list(MemChunkList* chunk_list) {

    if(chunk_list->current_size == 0) return;

    size_t i;
    for(i = 0; i < chunk_list->current_size - 1; i++) {
        printf("size: %ld, start: %p, end: %p | ", chunk_list->chunks[i].size, chunk_list->chunks[i].start, chunk_list->chunks[i].start + chunk_list->chunks[i].size);
    }
    printf("size: %ld, start: %p, end: %p", chunk_list->chunks[i].size, chunk_list->chunks[i].start, chunk_list->chunks[i].start + chunk_list->chunks[i].size);

    putchar('\n');
}
