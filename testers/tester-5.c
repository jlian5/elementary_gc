/**
 * malloc
 * CS 241 - Spring 2021
 */
#include "gc.h"

#include <stdlib.h>
#include <time.h>

#define K 10
#define MIN_ALLOC_SIZE 24
#define MAX_ALLOC_SIZE 100 * K
#define CHANCE_OF_FREE 95
#define CHANCE_OF_gc_realloc 50
#define TOTAL_ALLOCS 400

int main(void) {
    GC_INIT();
    gc_malloc(1);

    time_t t;
    srand((unsigned int) time(&t));

    int i;
    void *gc_realloc_ptr = NULL;
    void **dictionary = gc_malloc(TOTAL_ALLOCS * sizeof(void *));
    int *dictionary_elem_size = gc_malloc(TOTAL_ALLOCS * sizeof(int));
    int dictionary_ct = 0;
    int data_written = 0;

    for (i = 0; i < TOTAL_ALLOCS; i++) {
        int size =
            (rand() % (MAX_ALLOC_SIZE - MIN_ALLOC_SIZE + 1)) + MIN_ALLOC_SIZE;
        void *ptr;

        if (gc_realloc_ptr == NULL) {
            ptr = gc_malloc(size);
            data_written = 0;
        } else {
            ptr = gc_realloc(gc_realloc_ptr, size);
            gc_realloc_ptr = NULL;
        }

        if (ptr == NULL) {
            GC_EXIT(1, {puts("Memory failed to allocate!\n");});
            return 1;
        }

        if (rand() % 100 < CHANCE_OF_FREE)
            gc_free(ptr);
        else {
            if (!data_written) {
                *((void **)ptr) = &dictionary[dictionary_ct];
                data_written = 1;
            }

            if (rand() % 100 < CHANCE_OF_gc_realloc)
                gc_realloc_ptr = ptr;
            else {
                *((void **)(ptr + size - sizeof(void *))) =
                    &dictionary[dictionary_ct];
                dictionary[dictionary_ct] = ptr;
                dictionary_elem_size[dictionary_ct] = size;
                dictionary_ct++;
            }
        }
    }

    for (i = dictionary_ct - 1; i >= 0; i--) {
        if (*((void **)dictionary[i]) != &dictionary[i]) {
            GC_EXIT(100, {puts("Memory failed to contain correct data after many allocations "
                "(beginning of segment)!\n");});
            return 100;
        }

        if (*((void **)(dictionary[i] + dictionary_elem_size[i] -
                        sizeof(void *))) != &dictionary[i]) {
            GC_EXIT(101, {puts(
                "Memory failed to contain correct data after many allocations "
                "(end of segment)!\n");});
        }

        gc_free(dictionary[i]);
    }

    GC_EXIT(0, {puts("Memory was allocated and // freed!\n");});
}
