#include <stdio.h>
#include "n_allocator.h"
#include "mem_chunk.h"
#include <stdlib.h>

#define PRINT_DELIMITER printf("-----------------------------------------------------------------------------------------\n")
#define PRINT_ALLOC_ERROR printf("Error when allocating. Exiting.")
#define PRINT_FREE_ERROR printf("Error when freeing. Exiting.")

void print_all() {
    printf("HEAP: ");
    print_heap();
	printf("ALLOCATED CHUNKS:\n");
	print_chunk_list(&alloced_chunks);
	printf("FREE CHUNKS:\n");
	print_chunk_list(&free_chunks);
    PRINT_DELIMITER;
}

int main(int argc, char* argv[]) {
	init();
	printf("BEGINNING:\n");
	print_all();
	size_t o1_size = 1;
	char* o1 = (char*)n_alloc(o1_size);
	if(o1 == NULL) {
        PRINT_ALLOC_ERROR;
		exit(1);
	}
	print_all();

	size_t o2_size = 20;
	char* o2 = (char*)n_alloc(o2_size);
	if(o2 == NULL) {
        PRINT_ALLOC_ERROR;
		exit(1);
	}
	print_all();

	size_t o3_size = 5;
	char* o3 = (char*)n_alloc(o3_size);
	if(o3 == NULL) {
        PRINT_ALLOC_ERROR;
		exit(1);
	}
	print_all();

	int free_o2 = n_free(o2);
	if(free_o2 == -1) {
        PRINT_FREE_ERROR;
		exit(1);
	}
	print_all();

	size_t o4_size = 9;
	char* o4 = (char*)n_alloc(o4_size);
	if(o4 == NULL) {
        PRINT_ALLOC_ERROR;
		exit(1);
	}
	print_all();
	
	int free_o3 = n_free(o3);
	if(free_o3 == -1) {
        PRINT_FREE_ERROR;
		exit(1);
	}
	print_all();

	int free_o4 = n_free(o4);
	if(free_o4 == -1) {
        PRINT_FREE_ERROR;
		exit(1);
	}
	print_all();

	int free_o1 = n_free(o1);
	if(free_o1 == -1) {
        PRINT_FREE_ERROR;
		exit(1);
	}
	print_all();

	printf("SUCCESS\n");
	return 0;
}
