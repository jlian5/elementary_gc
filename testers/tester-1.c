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
            fprintf(stderr, "Memory failed to allocate!\n");
            return 1;
        }

        *ptr = 4;
    }

    GC_EXIT(0, {fprintf(stdout,"done");});
    return 0;
}
