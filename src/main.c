#include <stdio.h>

#include "n_allocator.h"
#include "n_allocator_printer.h"

#define PRINT_DELIMITER printf("-----------------------------------------------------------------------------------------\n")
#define PRINT_ALLOC_ERROR printf("Error when allocating. Exiting.\n")
#define PRINT_FREE_ERROR printf("Error when freeing. Exiting.\n")

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

    printf("BEGINNING STATE:\n");
    print_chunks();

    int* array = (int*)nalloc(sizeof(int) * 250);
    if(array) {
        for(int j = 0; j < 1000; j++) {
            array[j] = j * 134;
        }
        for(int j = 0; j < 1000; j++) {
            printf("%d ", array[j]);
        }
        putchar('\n');
        print_chunks();
    }

    char* s = (char*)nalloc(2000);
    if(s) {
        for(int j = 0; j < 2000-1; j++) {
            s[j] = j % 26 + 'a';
        }

        s[2000-1] = '\0';
        for(int j = 0; j < 2000; j++) {
            printf("%c ", s[j]);
        }
        putchar('\n');
        print_chunks();
    }

    s = nrealloc(s, 500);
    print_chunks();

    array = nrealloc(array, 500);
    print_chunks();
    array = nrealloc(array, 1000);
    print_chunks();
    array = nrealloc(array, 1500);
    print_chunks();
    int* temp = nrealloc(array, 1501);
    print_chunks();
    array = nrealloc(array, 500);
    print_chunks();

    char* s2 = nalloc(600);
    s2[599] = '\0';
    print_chunks();

    char* s3 = nalloc(133);
    char* s4 = nalloc(133);
    char* s5 = nalloc(134);
    print_chunks();

    nfree(s2);
    print_chunks();
    nfree(s4);
    print_chunks();
    nfree(s3);
    print_chunks();
    nfree(array);
    print_chunks();
    nfree(s);
    print_chunks();
    nfree(s5);
    print_chunks();
}
