#ifndef N_ALLOCATOR_INTERNAL_H
#define N_ALLOCATOR_INTERNAL_H

#include "mem_chunk_list.h"

extern MemChunkList alloced_chunks;
extern MemChunkList free_chunks;

struct Adj_Free_Chunks_Object {
    int left_ind;
    int right_ind;
};

void nfree_by_ind(size_t removed_chunk_ind);

void shrink_chunk(size_t chunk_for_shr_ind, MemChunkList* chunk_for_shr_list, size_t shr_amount);
void expn_chunk_into(MemChunk* chunk_for_expn, size_t chunk_to_expn_into_ind, MemChunkList* chunk_to_expn_into_list, size_t expn_amount);
void merge_into_first_chunk(MemChunk* chunk_for_expn, size_t chunk_to_expn_into_ind, MemChunkList* chunk_to_expn_into_list);

struct Adj_Free_Chunks_Object find_adj_free_chunks_for_free(size_t free_chunk_ind);
int merge_adj_free_chunks(size_t center_chunk_ind);
int find_adj_free_chunk_for_alloced(MemChunk* alloced_chunk);

void cpy_chunk_content(MemChunk* chunk, void* new_start);

#endif
