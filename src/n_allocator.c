#include <stdio.h>
#include "n_allocator.h"

#define ENABLE_DEBUG 1
#define DEBUG_ALLOC(x) if(ENABLE_DEBUG) printf("Allocating size: %ld\n", x);
#define DEBUG_FREE(x) if(ENABLE_DEBUG) printf("Freeing size: %ld\n", x);

MemChunkList mem_chunks;
MemChunkList alloced_chunks = {0};
MemChunkList free_chunks = {0};

void nalloc_init() {
    MemChunk free_heap = {
        .start = heap,
        .size = HEAP_CAPACITY
    };

    append_chunk(&free_chunks, &free_heap);
}

// First-fit algorithm
int find_free_space(size_t size) {
    size_t i;
    for(i = 0; i < free_chunks.current_size; i++) {
        if(size <= free_chunks.chunks[i].size) return i;
    }

    return -1;
}

int shrink_chunk(size_t chunk_for_shr_ind, MemChunkList* chunk_for_shr_list, size_t shr_amount) {
    //assert index
    //assert amount
    if(shr_amount == 0) return 0;

    MemChunk* chunk_for_shr = &(chunk_for_shr_list->chunks[chunk_for_shr_ind]);

    if(chunk_for_shr->size == shr_amount) {
        remove_chunk(chunk_for_shr_list, chunk_for_shr_ind);
        return 0;
    }
    else if(chunk_for_shr->size > shr_amount) {
        chunk_for_shr->size -= shr_amount;
        chunk_for_shr->start += shr_amount;
        return 0;
    }
    else return -1;
}

int expand_chunk_into(MemChunk* chunk_for_expn, size_t chunk_to_expn_into_ind, MemChunkList* chunk_to_expn_into_list, size_t expn_amount) {
    //assert indices
    //assert adjacency
    //assert amount
    if(expn_amount == 0) return 0;

    MemChunk* chunk_to_expn_into = &(chunk_to_expn_into_list->chunks[chunk_to_expn_into_ind]);

    if(get_right_chunk(chunk_for_expn, chunk_to_expn_into) == chunk_for_expn)
        chunk_for_expn->start -= expn_amount;

    shrink_chunk(chunk_to_expn_into_ind, chunk_to_expn_into_list, expn_amount);
    chunk_for_expn->size += expn_amount;

    return 0;
}

int merge_into_first_chunk(MemChunk* chunk_for_expn, size_t chunk_to_expn_into_ind, MemChunkList* chunk_to_expn_into_list) {
    // assert index
    MemChunk* chunk_to_expn_into = &(chunk_to_expn_into_list->chunks[chunk_to_expn_into_ind]);
    return expand_chunk_into(chunk_for_expn, chunk_to_expn_into_ind, chunk_to_expn_into_list, chunk_to_expn_into->size);
}

void* nalloc(size_t size) {
    DEBUG_ALLOC(size);
    if(size == 0) return NULL;

    int free_spot_ind = find_free_space(size);
    if(free_spot_ind == -1) return NULL;

    MemChunk* free_spot = &free_chunks.chunks[free_spot_ind];

    void* chunk_start = free_spot->start;
    int shrank_status = shrink_chunk(free_spot_ind, &free_chunks, size);
    if(shrank_status == -1) return NULL;

    MemChunk new = {
        .start = chunk_start,
        .size = size
    };
    return ((insert_chunk_by_addr(&alloced_chunks, &new) != -1) ? chunk_start : NULL);
}

struct Adj_Free_Chunks_Object {
    int left_ind;
    int right_ind;
};

struct Adj_Free_Chunks_Object find_adj_free_chunks_for_free(size_t free_chunk_ind) {
    // assert index

    int left_chunk_ind = free_chunk_ind - 1;
    int right_chunk_ind = free_chunk_ind + 1;

    if((left_chunk_ind < 0) || (!are_chunks_adj(&free_chunks.chunks[left_chunk_ind], &free_chunks.chunks[free_chunk_ind])));
        left_chunk_ind = -1;

    if(((size_t)right_chunk_ind >= free_chunks.current_size) || (!are_chunks_adj(&free_chunks.chunks[right_chunk_ind], &free_chunks.chunks[free_chunk_ind])))
        right_chunk_ind = -1;

    struct Adj_Free_Chunks_Object adj_free_chunks = {
        .left_ind = left_chunk_ind,
        .right_ind = right_chunk_ind
    };

    return adj_free_chunks;
}

struct Adj_Free_Chunks_Object find_adj_free_chunks_for_alloced(MemChunk* alloced_chunk) {
    // assert alloced_chunk != NULL
    size_t i;
    size_t left_closest_ind = -1;
    size_t right_closest_ind = -1;

    MemChunk* curr_chunk;
    for(i = 0; i < free_chunks.current_size; i++) {
        curr_chunk = &free_chunks.chunks[i];
        if(get_left_chunk(curr_chunk, alloced_chunk) == curr_chunk)
            left_closest_ind = i;
        else if(get_right_chunk(curr_chunk, alloced_chunk) == curr_chunk) {
            right_closest_ind = i;
            break;
        }
    }

    if(!are_chunks_adj(&free_chunks.chunks[left_closest_ind], alloced_chunk))
        left_closest_ind = -1;

    if(!are_chunks_adj(&free_chunks.chunks[right_closest_ind], alloced_chunk))
        right_closest_ind = -1;
        
    struct Adj_Free_Chunks_Object adj_free_chunks = {
        .left_ind = left_closest_ind,
        .right_ind = right_closest_ind
    };

    return adj_free_chunks;
}

int merge_adj_free_chunks(size_t center_chunk_ind) {
    // assert index
    struct Adj_Free_Chunks_Object adj_free_chunks = find_adj_free_chunks_for_free(center_chunk_ind);

    if(adj_free_chunks.right_ind != -1)
        merge_into_first_chunk(&(free_chunks.chunks[center_chunk_ind]), adj_free_chunks.right_ind, &free_chunks);
    if(adj_free_chunks.left_ind != -1)
        merge_into_first_chunk(&(free_chunks.chunks[adj_free_chunks.left_ind]), center_chunk_ind, &free_chunks);

    return 0;
}

int nfree(void* ptr) {
    if(ptr == NULL) return -1;

    int removed_chunk_ind = find_chunk_ind(&alloced_chunks, ptr);
    if(removed_chunk_ind == -1) return -1;

    DEBUG_FREE(alloced_chunks.chunks[removed_chunk_ind].size);

    MemChunk removed_chunk_cpy = alloced_chunks.chunks[removed_chunk_ind];

    int removed_status = remove_chunk(&alloced_chunks, removed_chunk_ind);
    if(removed_status == -1) return -1;

    int free_chunks_ind = insert_chunk_by_addr(&free_chunks, &removed_chunk_cpy);
    if(free_chunks_ind == -1) return -1;

    int merged = merge_adj_free_chunks(free_chunks_ind);

    return 0;

}

int check_expn_of_alloced_into_adj_free(size_t expn_amount, struct Adj_Free_Chunks_Object adj_free_chunks) {
    size_t adj_sum = 0;

    if(adj_free_chunks.left_ind != -1) adj_sum += free_chunks.chunks[adj_free_chunks.left_ind].size;
    if(adj_free_chunks.right_ind != -1) adj_sum += free_chunks.chunks[adj_free_chunks.right_ind].size;

    return ((expn_amount <= adj_sum) ? 1 : 0);
}

int expn_alloced_chunk_into_adj_free(MemChunk* alloced_chunk, struct Adj_Free_Chunks_Object adj_free_chunks, size_t expn_amount) {

    //assert expn_amount != 0
    
    int left_chunk_ind = adj_free_chunks.left_ind;
    int right_chunk_ind = adj_free_chunks.right_ind;

    MemChunk* left_chunk = ((left_chunk_ind != -1) ? &free_chunks.chunks[left_chunk_ind] : NULL);
    MemChunk* right_chunk = ((right_chunk_ind != -1) ? &free_chunks.chunks[right_chunk_ind] : NULL);
    
    size_t expn_amount_into_left_chunk = 0, expn_amount_into_right_chunk = 0;
    if(left_chunk != NULL) {
        if(left_chunk->size >= expn_amount)
            expn_amount_into_left_chunk = expn_amount;
        else
            expn_amount_into_left_chunk = left_chunk->size;

        expn_amount -= expn_amount_into_left_chunk;
    }

    if(right_chunk != NULL) {
        if(right_chunk->size >= expn_amount)
            expn_amount_into_right_chunk = expn_amount;
        else
            expn_amount_into_right_chunk = right_chunk->size;
        expn_amount -= expn_amount_into_right_chunk;
    }

    // assert expn_amount = 0;

    //expand_chunk(MemChunk* chunk_for_expn, size_t chunk_to_expn_into_ind, MemChunkList* chunk_to_expand_into_list, size_t expn_amount)
    if(right_chunk != NULL)
        expand_chunk_into(alloced_chunk, right_chunk_ind, &free_chunks, expn_amount_into_right_chunk);
    if(left_chunk != NULL) {
        expand_chunk_into(alloced_chunk, left_chunk_ind, &free_chunks, expn_amount_into_left_chunk);
    }

}

void* nrealloc(void* ptr, size_t new_size) {
    if(ptr == NULL) return NULL;
       
    printf("0\n");
    int chunk_index = find_chunk_ind(&alloced_chunks, ptr);
    printf("05\n");
    if(chunk_index == -1) return NULL;
    printf("1\n");

    MemChunk* chunk_for_nrealloc = &alloced_chunks.chunks[chunk_index];

    struct Adj_Free_Chunks_Object adj_free_chunks = find_adj_free_chunks_for_alloced(chunk_for_nrealloc);
    printf("2\n");

    size_t expn_amount = new_size - chunk_for_nrealloc->size;
    printf("3\n");

    if(check_expn_of_alloced_into_adj_free(expn_amount, adj_free_chunks)) {
        expn_alloced_chunk_into_adj_free(chunk_for_nrealloc, adj_free_chunks, expn_amount);
        printf("4\n");
    }
    printf("5\n");

    return ptr;
}

void print_chunks() {
    printf("ALLOCED CHUNKS:\n");
    if(alloced_chunks.current_size == 0) printf("empty\n");
    else print_chunk_list(&alloced_chunks);
    printf("FREE CHUNKS:\n");
    if(free_chunks.current_size == 0) printf("empty\n");
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
    while((free_count < free_chunks.current_size) && (alloced_count < alloced_chunks.current_size)) {
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

    for(; free_count < free_chunks.current_size; free_count++)
        visualize_chunk(&free_chunks.chunks[free_count], FREE_IND);

    for(; alloced_count < alloced_chunks.current_size; alloced_count++)
        visualize_chunk(&alloced_chunks.chunks[alloced_count], ALLOCED_IND);
}

