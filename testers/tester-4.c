/**
 * malloc
 * CS 241 - Spring 2021
 */
#include "gc.h"

#define G 100
#define K 10
#define START_MALLOC_SIZE (1 * G)
#define STOP_MALLOC_SIZE (1 * K)

void *reduce(void *ptr, int size) {
    if (size > STOP_MALLOC_SIZE) {
        void *ptr1 = gc_realloc(ptr, size / 2);
        void *ptr2 = gc_malloc(size / 2);

        if (ptr1 == NULL || ptr2 == NULL) {
            GC_EXIT(1, {puts("Memory failed to allocate!\n");});
            exit(1);
        }

        ptr1 = reduce(ptr1, size / 2);
        ptr2 = reduce(ptr2, size / 2);

        if (*((int *)ptr1) != size / 2 || *((int *)ptr2) != size / 2) {
            GC_EXIT(2,{puts("Memory failed to contain correct data after many "
                            "allocations!\n");});
        }

        // free(ptr2);
        ptr1 = gc_realloc(ptr1, size);

        if (*((int *)ptr1) != size / 2) {
            GC_EXIT(3, {puts("Memory failed to contain correct data after gc_realloc()!\n");});
        }

        *((int *)ptr1) = size;
        GC_RETURN(ptr1, {;});
    } else {
        *((int *)ptr) = size;
        GC_RETURN(ptr, {;});
    }
}

int main(void) {
    GC_INIT();
    gc_malloc(1);

    int size = START_MALLOC_SIZE;
    while (size > STOP_MALLOC_SIZE) {
        void *ptr = gc_malloc(size);
        ptr = reduce(ptr, size / 2);
        // free(ptr);

        size /= 2;
    }

    fprintf(stderr, "Memory was allocated, used, and freed!\n");
    GC_EXIT(0, {puts("end of main");});
    return 0;
}
