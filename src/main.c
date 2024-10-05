#include <stdio.h>
#include "n_allocator.h"
#include "mem_chunk.h"
#include <stdlib.h>
void printAllAndFree() {
	printf("ALLOCATED CHUNKS:\n");
	print_mem_chunk_list(&allocated_chunks);
	printf("FREE CHUNKS:\n");
	print_mem_chunk_list(&free_chunks);
}

int main(int argc, char* argv[]) {
	init();
	printf("BEGINNING STATE\n");
	print_heap();
	printAllAndFree();





	size_t o1_size = 1;
	char* o1 = (char*)n_allocate(o1_size);
	if(o1 == NULL) {
		printf("ERROR! Exiting!\n");
		exit(1);
	}
	printf("\n---------------------------------------------------------------\nAllocated o1 - size: %ld\n", o1_size);
	print_heap();
	printAllAndFree();





	size_t o2_size = 20;
	char* o2 = (char*)n_allocate(o2_size);
	if(o2 == NULL) {
		printf("ERROR! Exiting!\n");
		exit(1);
	}
	printf("\n---------------------------------------------------------------\nAllocated o2 - size: %ld\n", o2_size);
	print_heap();
	printAllAndFree();

	size_t o3_size = 5;
	char* o3 = (char*)n_allocate(o3_size);
	if(o3 == NULL) {
		printf("ERROR! Exiting!\n");
		exit(1);
	}
	printf("\n---------------------------------------------------------------\nAllocated o3 - size: %ld\n", o3_size);
	print_heap();
	printAllAndFree();

	int free_o2 = n_free(o2);
	if(free_o2 == -1) {
		printf("ERROR WHEN FREEING! Exiting!\n");
		exit(1);
	}
	printf("\n---------------------------------------------------------------\nFREED O2\n");
	print_heap();
	printAllAndFree();

	size_t o4_size = 9;
	char* o4 = (char*)n_allocate(o4_size);
	if(o4 == NULL) {
		printf("ERROR! Exiting!\n");
		exit(1);
	}
	printf("---------------------------------------------------------------\nAllocated o4 - size %ld\n", o4_size);
	print_heap();
	printAllAndFree();
	
	int free_o3 = n_free(o3);
	if(free_o3 == -1) {
		printf("ERROR WHEN FREEING! Exiting!\n");
		exit(1);
	}
	printf("---------------------------------------------------------------\nFREED O3\n");
	print_heap();
	printAllAndFree();

	int o4_free = n_free(o4);
	if(o4_free == -1) {
		printf("ERROR WHEN FREEING! Exiting!\n");
		exit(1);
	}
	printf("%d---------------------------------------------------------------\nFREED o4\n", o4_free);
	print_heap();
	printAllAndFree();

	int o1_free = n_free(o1);
	if(o1_free == -1) {
		printf("ERROR WHEN FREEING! Exiting!\n");
		exit(1);
	}
	printf("%d---------------------------------------------------------------\nFREED o1\n", o1_free);
	print_heap();
	printAllAndFree();

	printf("SUCCESS\n");
	return 0;
}
