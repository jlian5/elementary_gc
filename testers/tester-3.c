/**
 * malloc
 * CS 241 - Spring 2021
 */
#include <stdio.h>

#include "gc.h"

#define TOTAL_ALLOCS 600
#define ALLOC_SIZE 100 * 100

int main(void) {
    GC_INIT();
    gc_malloc(1);

    int i;
    void *ptr = NULL;

    for (i = 0; i < TOTAL_ALLOCS; i++) {
        ptr = gc_malloc(ALLOC_SIZE);
        if (ptr == NULL) {
            fprintf(stderr, "Memory failed to allocate!\n");
            return 1;
        }

        memset(ptr, 0xab, ALLOC_SIZE);

        // free(ptr);
    }

    fprintf(stderr, "Memory was allocated and freed!\n");
    GC_EXIT(0,{puts("end of main");});
    return 0;
}
