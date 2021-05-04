/**
 * malloc
 * CS 241 - Spring 2021
 */
#include <stdio.h>

#include "gc.h"

#define NUM_CYCLES 100

int main(void) {
    GC_INIT();
    int i;
    for (i = 0; i < NUM_CYCLES; i++) {
        int *ptr = gc_malloc(sizeof(int));

        if (ptr == NULL) {
            GC_RETURN(1, {puts("Memory failed to allocate!\n");});
        }

        *ptr = 4;
    }

    GC_RETURN(0, {fprintf(stdout,"done");});
}
