#include <stdio.h>
#include "n_allocator.h"
#include "misc.h"

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

struct Adj_Free_Chunks_Object {
    int left_ind;
    int right_ind;
};

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

int merge_adj_free_chunks(size_t center_chunk_ind) {
    ASSERT(center_chunk_ind < free_chunks.cap, "Cannot merge adjacent free chunks for an invalid chunk index.");
    struct Adj_Free_Chunks_Object adj_free_chunks = find_adj_free_chunks_for_free(center_chunk_ind);

    if(adj_free_chunks.right_ind != -1)
        merge_into_first_chunk(&(free_chunks.chunks[center_chunk_ind]), adj_free_chunks.right_ind, &free_chunks);
    if(adj_free_chunks.left_ind != -1)
        merge_into_first_chunk(&(free_chunks.chunks[adj_free_chunks.left_ind]), center_chunk_ind, &free_chunks);

    return 0;
}

void nfree_by_ind(size_t removed_chunk_ind) {
    ASSERT(alloced_chunks.size < alloced_chunks.cap, "Alloced chunks reached max capacity.");

    DEBUG_FREE(alloced_chunks.chunks[removed_chunk_ind].size);
    ASSERT(removed_chunk_ind < alloced_chunks.size, "Invalid chunk for removal index");

    MemChunk removed_chunk_cpy = alloced_chunks.chunks[removed_chunk_ind];

    int removed_status = remove_chunk(&alloced_chunks, removed_chunk_ind);
    ASSERT(removed_status == 0, "Removing chunk from alloced chunks failed.");

    int free_chunks_ind = insert_chunk_by_addr(&free_chunks, &removed_chunk_cpy);
    ASSERT(free_chunks_ind >= 0, "Inserting previously alloced chunk into free chunks failed.");

    int merged_status = merge_adj_free_chunks(free_chunks_ind);
    ASSERT(merged_status == 0, "Merging adjacent free chunks failed");

}

int nfree(void* ptr) {
    if(ptr == NULL) return -1;
    if(free_chunks.size >= free_chunks.cap) return -1;

    int removed_chunk_ind = find_chunk_ind(&alloced_chunks, ptr);
    if(removed_chunk_ind == -1) return -1;

    nfree_by_ind(removed_chunk_ind);

    return 0;
}

void cpy_chunk_content(MemChunk* chunk, void* new_start) {
    char* it = (char*)chunk->start;
    char* it_new = (char*)new_start;
    void* end = chunk->start + chunk->size;

    for(; it < (char*)end; it++, it_new++)
       (*it_new) = (*it);
}

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

void* nrealloc(void* ptr, size_t new_size) {
    if(ptr == NULL) return NULL;
    DEBUG_NREALLOC(new_size);
       
    int chunk_for_nrealloc_ind = find_chunk_ind(&alloced_chunks, ptr);
    if(chunk_for_nrealloc_ind == -1) return NULL;

    MemChunk* chunk_for_nrealloc = &alloced_chunks.chunks[chunk_for_nrealloc_ind];

    int expn_amount = new_size - chunk_for_nrealloc->size;
    if(expn_amount == 0) return ptr;

    return ((expn_amount > 0) ? handle_nrealloc_expn(chunk_for_nrealloc_ind, new_size) : handle_nrealloc_shr(chunk_for_nrealloc, -expn_amount));

}

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

