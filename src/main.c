#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "n_allocator.h"

#define PRINT_DELIMITER printf("-----------------------------------------------------------------------------------------\n")
#define PRINT_ALLOC_ERROR printf("Error when allocating. Exiting.\n")
#define PRINT_FREE_ERROR printf("Error when freeing. Exiting.\n")
#define ATTEMPT_IMPOSSIBLE_ALLOC 0

struct TestStruct {
	int x;
	int y;
};

void print_all() {
    printf("HEAP:\n");
    visualize_heap();
	putchar('\n');
	print_chunks();
    PRINT_DELIMITER;
}

int main(int argc, char* argv[]) {
	nalloc_init();

	printf("BEGINNING:\n");
	print_all();

	// alloc: string1 ---------------------------------------------------------------------

	int string1_size = 7;
	printf("Allocating string(length: %d) and filling with chars.\n", string1_size);
	char* string1 = (char*)n_alloc(string1_size * sizeof(char));
	if(string1 == NULL) {
		PRINT_ALLOC_ERROR;
		exit(1);
	};

	strcpy(string1, "nalloc");
	string1[string1_size-1] = '\0';

	printf("string1: %s\n", string1);

	print_all();

	// alloc: array1 ---------------------------------------------------------------------

	int array1_size = 12;
	printf("Allocating int array(length: %d) and filling with ints.\n", array1_size);
	int* array1 = (int*)n_alloc(array1_size * sizeof(int));
	if(array1 == NULL) {
		PRINT_ALLOC_ERROR;
		exit(1);
	};

	int i;
	for(i = 0; i < array1_size; i++) array1[i] = i + 10;

	printf("array1: ");
	for(i = 0; i < array1_size; i++) printf("%d ", array1[i]);
	putchar('\n');

	print_all();

	// alloc: teststruct1 ----------------------------------------------------------------

	int x = 29;
	int y = 24;

	printf("Allocating test struct and filling with ints: %d %d.\n", x, y);
	struct TestStruct* tst1 = (struct TestStruct*)n_alloc(sizeof(struct TestStruct));
	if(tst1 == NULL) {
		PRINT_ALLOC_ERROR;
		exit(1);
	};

	tst1->x = x;
	tst1->y = y;

	printf("test struct1 - x: %d, y: %d\n", tst1->x, tst1->y);
	print_all();

	// free: array1 ----------------------------------------------------------------------

	printf("Freeing int array(length: %d)\n", array1_size);
	int arr1_free_status = n_free(array1);
	if(arr1_free_status == -1) {
		PRINT_FREE_ERROR;
		exit(1);
	}

	print_all();

	// free: teststruct1 -----------------------------------------------------------------

	printf("Freeing test struct1\n");
	int tst1_free_status = n_free(tst1);
	if(arr1_free_status == -1) {
		PRINT_FREE_ERROR;
		exit(1);
	}

	print_all();

	// free: string1 ---------------------------------------------------------------------
	
	printf("Freeing string(length: %d)\n", string1_size);

	int str1_free_status = n_free(string1);
	if(str1_free_status == -1) {
		PRINT_FREE_ERROR;
		exit(1);
	}

	print_all();

	// allocate too large ----------------------------------------------------------------

	if(ATTEMPT_IMPOSSIBLE_ALLOC) {
		size_t alloc_amount = sizeof(struct TestStruct) * 500;
		printf("Allocating size: %d, even though heap capacity is: %ld\n", alloc_amount, HEAP_CAPACITY);

		struct TestStruct* tst2 = (struct TestStruct*)n_alloc(alloc_amount);
		if(tst2 == NULL) {
			PRINT_ALLOC_ERROR;
			exit(1);
		};

		print_all();
	}

	printf("SUCCESS\n");
	return 0;
}
