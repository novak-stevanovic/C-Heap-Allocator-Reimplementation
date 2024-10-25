#include "n_reallocator.h"
#include "misc.h"
#include "mem_chunk.h"
#include "mem_chunk_list.h"
#include "n_allocator.h"
#include "n_allocator_shared.h"

void* handle_partial_nrealloc_expn(MemChunk* chunk_for_nrealloc, size_t right_chunk_ind, size_t expn_amount) {
    ASSERT((size_t)right_chunk_ind < free_chunks.size, "Invalid right chunk index in free chunks list.");
    expn_chunk_into(chunk_for_nrealloc, right_chunk_ind, &free_chunks, expn_amount);
    return chunk_for_nrealloc->start;
}

void* handle_complete_nrealloc_expn(size_t chunk_for_nrealloc_ind, size_t new_size) {
    MemChunk* chunk_for_nrealloc = &alloced_chunks.chunks[chunk_for_nrealloc_ind];
    void* new_ptr;

    if((free_chunks.size < free_chunks.cap) && (alloced_chunks.size < alloced_chunks.cap)) { // free_chunks and alloced_chunks are not at max capacity
        if(alloced_chunks.size >= alloced_chunks.cap) new_ptr = NULL; // if no more chunks can be alloced, don't do anything
        else {
            new_ptr = nalloc(new_size);
            if(new_ptr != NULL) {
                nfree_by_ind(chunk_for_nrealloc_ind);

                if(new_ptr != NULL) cpy_chunk_content(chunk_for_nrealloc, new_ptr);
            }
        }

    }
    else new_ptr = NULL; // if they are, new_ptr stays null

    return new_ptr;
}

void* handle_nrealloc_expn(size_t chunk_for_nrealloc_ind, size_t new_size) {
    MemChunk* chunk_for_nrealloc = &alloced_chunks.chunks[chunk_for_nrealloc_ind];
    int right_chunk_ind = find_adj_free_chunk_for_alloced(chunk_for_nrealloc);

    size_t expn_amount = new_size - chunk_for_nrealloc->size;

    if((right_chunk_ind != -1) && (free_chunks.chunks[right_chunk_ind].size >= (size_t)expn_amount)) // free chunk right of expanding chunk exists and has enough size
        return handle_partial_nrealloc_expn(chunk_for_nrealloc, right_chunk_ind, expn_amount);
    else // complete reallocation 
        return handle_complete_nrealloc_expn(chunk_for_nrealloc_ind, new_size);
}

void* handle_nrealloc_shr(MemChunk* chunk_for_nrealloc, size_t shr_amount) {
    if(free_chunks.size >= free_chunks.cap) return NULL;

    chunk_for_nrealloc->size -= shr_amount;

    MemChunk new_free_chunk = {
        .start = chunk_for_nrealloc->start + chunk_for_nrealloc->size,
        .size = shr_amount
    };

    int new_free_chunk_ind = insert_chunk_by_addr(&free_chunks, &new_free_chunk);
    merge_adj_free_chunks(new_free_chunk_ind);

    return chunk_for_nrealloc->start;
}

void* nreallocator_nrealloc(void* ptr, size_t new_size) {
    if(ptr == NULL) return NULL;
       
    int chunk_for_nrealloc_ind = find_chunk_ind(&alloced_chunks, ptr);
    if(chunk_for_nrealloc_ind == -1) return NULL;

    MemChunk* chunk_for_nrealloc = &alloced_chunks.chunks[chunk_for_nrealloc_ind];

    int expn_amount = new_size - chunk_for_nrealloc->size;
    if(expn_amount == 0) return ptr;

    return ((expn_amount > 0) ? handle_nrealloc_expn(chunk_for_nrealloc_ind, new_size) : handle_nrealloc_shr(chunk_for_nrealloc, -expn_amount));
}
