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
            GC_RETURN(1, {puts("Memory failed to allocate!\n");});
        }

        memset(ptr, 0xab, ALLOC_SIZE);

        gc_free(ptr);
    }

    GC_EXIT(0,{puts("Memory was allocated and freed!\n");});
}
