/**
 * malloc
 * CS 241 - Spring 2021
 */
#include <stdio.h>

#include "gc.h"

#define TOTAL_ALLOCS 5 // * 1024 * 1024

int main(void) {
    GC_INIT();
    gc_malloc(1);

    int i;
    int **arr = gc_malloc(TOTAL_ALLOCS * sizeof(int *));
    if (arr == NULL) {
        GC_RETURN(1, {puts("Memory failed to allocate!\n");});
    }

    for (i = 0; i < TOTAL_ALLOCS; i++) {
        arr[i] = gc_malloc(sizeof(int));
        if (arr[i] == NULL) {
            GC_RETURN(1, {puts("Memory failed to allocate!\n");});
        }

        *(arr[i]) = i;
    }

    for (i = 0; i < TOTAL_ALLOCS; i++) {
        if (*(arr[i]) != i) {
            GC_RETURN(2, {puts("Memory failed to contain correct data after many allocations!\n");});
        }
    }

    GC_RETURN(0, {puts("done");});
}
