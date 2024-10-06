#include "mem_chunk.h"

int are_chunks_adj(MemChunk* chunk1, MemChunk* chunk2) {
    MemChunk* left_chunk = get_left_chunk(chunk1, chunk2);
    MemChunk* right_chunk = get_right_chunk(chunk1, chunk2);

    return ((left_chunk->start + left_chunk->size) == right_chunk->start);
}

MemChunk* get_left_chunk(MemChunk* chunk1, MemChunk* chunk2) {
    if(chunk1->start == chunk2->start) return NULL;

    return (chunk1->start < chunk2->start ? chunk1 : chunk2);

}

MemChunk* get_right_chunk(MemChunk* chunk1, MemChunk* chunk2) {
    if(chunk1->start == chunk2->start) return NULL;

    return (chunk1->start > chunk2->start ? chunk1 : chunk2);
}
