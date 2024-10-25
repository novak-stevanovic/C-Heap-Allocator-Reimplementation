#include <stdio.h>
#include "n_allocator_internal.h"
#include "misc.h"

void shrink_chunk(size_t chunk_for_shr_ind, MemChunkList* chunk_for_shr_list, size_t shr_amount) {
    ASSERT(chunk_for_shr_ind < chunk_for_shr_list->size, "Invalid chunk for shrinking index.");
    if(shr_amount == 0) return;

    MemChunk* chunk_for_shr = &(chunk_for_shr_list->chunks[chunk_for_shr_ind]);
    ASSERT(shr_amount <= chunk_for_shr->size, "Shrinkage amount must not be greater than chunk size.");

    if(chunk_for_shr->size == shr_amount)
        remove_chunk(chunk_for_shr_list, chunk_for_shr_ind);
    else if(chunk_for_shr->size > shr_amount) {
        chunk_for_shr->size -= shr_amount;
        chunk_for_shr->start += shr_amount;
    }
}

void expn_chunk_into(MemChunk* chunk_for_expn, size_t chunk_to_expn_into_ind, MemChunkList* chunk_to_expn_into_list, size_t expn_amount) {
    ASSERT(chunk_to_expn_into_ind < chunk_to_expn_into_list->size, "Invalid chunk to expand into index.");

    if(expn_amount == 0) return;

    MemChunk* chunk_to_expn_into = &(chunk_to_expn_into_list->chunks[chunk_to_expn_into_ind]);

    ASSERT(are_chunks_adj(chunk_for_expn, chunk_to_expn_into) == 1, "Chunks are not adjacent.");

    if(get_right_chunk(chunk_for_expn, chunk_to_expn_into) == chunk_for_expn)
        chunk_for_expn->start -= expn_amount;

    shrink_chunk(chunk_to_expn_into_ind, chunk_to_expn_into_list, expn_amount);
    chunk_for_expn->size += expn_amount;
}

void merge_into_first_chunk(MemChunk* chunk_for_expn, size_t chunk_to_expn_into_ind, MemChunkList* chunk_to_expn_into_list) {
    ASSERT(chunk_to_expn_into_ind < chunk_to_expn_into_list->size, "Invalud chunk to expand into index.");

    MemChunk* chunk_to_expn_into = &(chunk_to_expn_into_list->chunks[chunk_to_expn_into_ind]);
    expn_chunk_into(chunk_for_expn, chunk_to_expn_into_ind, chunk_to_expn_into_list, chunk_to_expn_into->size);
}

void cpy_chunk_content(MemChunk* chunk, void* new_start) {
    char* it = (char*)chunk->start;
    char* it_new = (char*)new_start;
    void* end = chunk->start + chunk->size;

    for(; it < (char*)end; it++, it_new++)
       (*it_new) = (*it);
}

struct Adj_Free_Chunks_Object find_adj_free_chunks_for_free(size_t free_chunk_ind) {
    ASSERT(free_chunk_ind < free_chunks.cap, "Invalid free chunk index.");

    int left_chunk_ind = free_chunk_ind - 1;
    int right_chunk_ind = free_chunk_ind + 1;

    if((left_chunk_ind < 0) || (!are_chunks_adj(&free_chunks.chunks[left_chunk_ind], &free_chunks.chunks[free_chunk_ind])))
        left_chunk_ind = -1;

    if(((size_t)right_chunk_ind >= free_chunks.size) || (!are_chunks_adj(&free_chunks.chunks[right_chunk_ind], &free_chunks.chunks[free_chunk_ind])))
        right_chunk_ind = -1;

    struct Adj_Free_Chunks_Object adj_free_chunks = {
        .left_ind = left_chunk_ind,
        .right_ind = right_chunk_ind
    };

    return adj_free_chunks;
}

int merge_adj_free_chunks(size_t center_chunk_ind) {
    ASSERT(center_chunk_ind < free_chunks.cap, "Cannot merge adjacent free chunks for an invalid chunk index.");
    struct Adj_Free_Chunks_Object adj_free_chunks = find_adj_free_chunks_for_free(center_chunk_ind);

    if(adj_free_chunks.right_ind != -1)
        merge_into_first_chunk(&(free_chunks.chunks[center_chunk_ind]), adj_free_chunks.right_ind, &free_chunks);
    if(adj_free_chunks.left_ind != -1)
        merge_into_first_chunk(&(free_chunks.chunks[adj_free_chunks.left_ind]), center_chunk_ind, &free_chunks);

    return 0;
}

int find_adj_free_chunk_for_alloced(MemChunk* alloced_chunk) {
    ASSERT(alloced_chunk != NULL, "Cannot find adjacent chunk for null chunk");

    size_t i;
    int right_closest_ind = -1;

    MemChunk* curr_chunk;
    for(i = 0; i < free_chunks.size; i++) {
        curr_chunk = &free_chunks.chunks[i];
        if(get_right_chunk(curr_chunk, alloced_chunk) == curr_chunk) {
            right_closest_ind = i;
            break;
        }
    }

    return (are_chunks_adj(alloced_chunk, &free_chunks.chunks[right_closest_ind]) ? right_closest_ind : -1);

}

void nfree_by_ind(size_t removed_chunk_ind) {
    ASSERT(alloced_chunks.size < alloced_chunks.cap, "Alloced chunks reached max capacity.");
    ASSERT(removed_chunk_ind < alloced_chunks.size, "Invalid chunk for removal index");

    MemChunk removed_chunk_cpy = alloced_chunks.chunks[removed_chunk_ind];

    int removed_status = remove_chunk(&alloced_chunks, removed_chunk_ind);
    ASSERT(removed_status == 0, "Removing chunk from alloced chunks failed.");

    int free_chunks_ind = insert_chunk_by_addr(&free_chunks, &removed_chunk_cpy);
    ASSERT(free_chunks_ind >= 0, "Inserting previously alloced chunk into free chunks failed.");

    int merged_status = merge_adj_free_chunks(free_chunks_ind);
    ASSERT(merged_status == 0, "Merging adjacent free chunks failed");

}


