#include <stdio.h>

#include "n_allocator_printer.h"
#include "n_allocator_shared.h"
#include "heap.h"

void print_chunks() {
    printf("ALLOCED CHUNKS:\n");
    if(alloced_chunks.size == 0) printf("empty\n");
    else print_chunk_list(&alloced_chunks);
    printf("FREE CHUNKS:\n");
    if(free_chunks.size == 0) printf("empty\n");
    else print_chunk_list(&free_chunks);
}

void visualize_chunk(MemChunk* chunk, char c) {
    size_t i;
    for(i = 0; i < chunk->size; i++)
        printf("%c ", c);
}

void visualize_heap() {
    if(HEAP_CAPACITY > VISUALIZE_HEAP_MAX) return;

    size_t free_count = 0, alloced_count = 0;
    while((free_count < free_chunks.size) && (alloced_count < alloced_chunks.size)) {
        MemChunk* curr_free_chunk = &free_chunks.chunks[free_count];
        MemChunk* curr_alloced_chunk = &alloced_chunks.chunks[alloced_count];

        if(get_left_chunk(curr_free_chunk, curr_alloced_chunk) == curr_free_chunk) {
            visualize_chunk(curr_free_chunk, FREE_IND);
            free_count++;
        }
        else {
            visualize_chunk(curr_alloced_chunk, ALLOCED_IND);
            alloced_count++;
        }
    }

    for(; free_count < free_chunks.size; free_count++)
        visualize_chunk(&free_chunks.chunks[free_count], FREE_IND);

    for(; alloced_count < alloced_chunks.size; alloced_count++)
        visualize_chunk(&alloced_chunks.chunks[alloced_count], ALLOCED_IND);
}

void print_chunk_count() {
    printf("ALLOCED CHUNKS: %ld\n", alloced_chunks.size);
    printf("FREE CHUNKS: %ld\n", free_chunks.size);
}
