#ifndef HEAP_H
#define HEAP_H

#include "mem_chunk.h"

#define HEAP_CAPACITY 80

#define FREE_IND 'o'
#define TAKEN_IND 'x'
#define UNKN_IND 0
#define ERR_IND 'U'

extern char heap[HEAP_CAPACITY];

void heap_init();
void print_heap();
void mark(MemChunk* chunk, char c);

#endif
