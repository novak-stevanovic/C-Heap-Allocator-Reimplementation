#include <stdio.h>
#include <stdlib.h>
#include "n_allocator.h"

#define PRINT_DELIMITER printf("-----------------------------------------------------------------------------------------\n")
#define PRINT_ALLOC_ERROR printf("Error when allocating. Exiting.")
#define PRINT_FREE_ERROR printf("Error when freeing. Exiting.")
#define PRINT_ALLOC(x) printf("Allocating size: %ld\n", x);
#define PRINT_FREE(x) printf("Freeing size: %ld\n", x);

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
	nalloc_init();
	heap_init();

	printf("BEGINNING:\n");
	print_all();

	// alloc: 1 ------------------------------------------------------------------------------------

	size_t o1_size = 1;
	PRINT_ALLOC(o1_size);
	char* o1 = (char*)n_alloc(o1_size);
	if(o1 == NULL) {
        PRINT_ALLOC_ERROR;
		exit(1);
	}
	print_all();

	// alloc: 20 ------------------------------------------------------------------------------------

	size_t o2_size = 20;
	PRINT_ALLOC(o2_size);
	char* o2 = (char*)n_alloc(o2_size);
	if(o2 == NULL) {
        PRINT_ALLOC_ERROR;
		exit(1);
	}
	print_all();

	// alloc: 20 ------------------------------------------------------------------------------------

	size_t o3_size = 5;
	PRINT_ALLOC(o3_size);
	char* o3 = (char*)n_alloc(o3_size);
	if(o3 == NULL) {
        PRINT_ALLOC_ERROR;
		exit(1);
	}
	print_all();

	// free: 20 -------------------------------------------------------------------------------------

	int free_o2 = n_free(o2);
	PRINT_FREE(o2_size);
	if(free_o2 == -1) {
        PRINT_FREE_ERROR;
		exit(1);
	}
	print_all();

	// alloc: 9 -------------------------------------------------------------------------------------

	size_t o4_size = 9;
	PRINT_ALLOC(o4_size);
	char* o4 = (char*)n_alloc(o4_size);
	if(o4 == NULL) {
        PRINT_ALLOC_ERROR;
		exit(1);
	}
	print_all();
	
	// alloc: 5 -------------------------------------------------------------------------------------

	int free_o3 = n_free(o3);
	PRINT_FREE(o3_size);
	if(free_o3 == -1) {
        PRINT_FREE_ERROR;
		exit(1);
	}
	print_all();

	// free: 9 --------------------------------------------------------------------------------------

	int free_o4 = n_free(o4);
	PRINT_FREE(o4_size);
	if(free_o4 == -1) {
        PRINT_FREE_ERROR;
		exit(1);
	}
	print_all();

	// free: 1 --------------------------------------------------------------------------------------

	int free_o1 = n_free(o1);
	PRINT_FREE(o1_size);
	if(free_o1 == -1) {
        PRINT_FREE_ERROR;
		exit(1);
	}
	print_all();

	printf("SUCCESS\n");
	return 0;
}
