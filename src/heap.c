#include <stddef.h>
#include "heap.h"
#include <stdio.h>

char heap[HEAP_CAPACITY] = {0};

void heap_init() {
	MemChunk temp = {
		.start = heap,
		.size = HEAP_CAPACITY
	};

	mark(&temp, FREE_IND);
}

int validate_state(char char_on_heap, char new_char) {
	if(char_on_heap == UNKN_IND) return 1;
	if((char_on_heap == FREE_IND) && (new_char == TAKEN_IND)) return 1;
	if((char_on_heap == TAKEN_IND) && (new_char == FREE_IND)) return 1;

	return 0;
}

void mark(MemChunk* chunk, char c) {
	char* it = chunk->start;
	size_t i;
	for(i = 0; i < chunk->size; it++, i++) {
		validate_state(*it, c) ? (*it) = c : (*it) == ERR_IND;
	}
}

void print_heap() {
	size_t i;
	for(i = 0; i < HEAP_CAPACITY; i++) {
        printf("%c ", heap[i]);
	}
	putchar('\n');
}
