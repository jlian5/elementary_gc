/**
 * malloc
 * CS 241 - Spring 2021
 */
#include "gc.h"
#include "tester-utils.h"

#define NUM_CYCLES 100000

int main(void) {
    int i;
    for (i = 0; i < NUM_CYCLES; i++) {
        int *ptr = gc_calloc(1, sizeof(int));
        verify_clean((char *)ptr, sizeof(int));

        if (ptr == NULL) {
            fprintf(stderr, "Memory failed to allocate!\n");
            GC_EXIT(1, {;});
        }

        *ptr = 4;
        gc_free(ptr);
    }

    fprintf(stderr, "Memory was allocated, used, and freed!\n");
    GC_EXIT(0, {;});
}
