/**
 * malloc
 * CS 241 - Spring 2021
 */
#include <stdio.h>

#include "../gc.h"

#define TOTAL_ALLOCS 5 // * 1024 * 1024

int main(int argc, char *argv[]) {
    GC_INIT();
    malloc(1);

    int i;
    int **arr = gc_malloc(TOTAL_ALLOCS * sizeof(int *));
    if (arr == NULL) {
        fprintf(stderr, "Memory failed to allocate!\n");
        return 1;
    }

    for (i = 0; i < TOTAL_ALLOCS; i++) {
        arr[i] = gc_malloc(sizeof(int));
        if (arr[i] == NULL) {
            fprintf(stderr, "Memory failed to allocate!\n");
            return 1;
        }

        *(arr[i]) = i;
    }

    for (i = 0; i < TOTAL_ALLOCS; i++) {
        if (*(arr[i]) != i) {
            fprintf(stderr, "Memory failed to contain correct data after many "
                            "allocations!\n");
            return 2;
        }
    }

    GC_EXIT(0, {;});
    return 0;
}
