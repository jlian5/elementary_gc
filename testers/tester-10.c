/**
 * malloc
 * CS 241 - Spring 2021
 */
#include "gc.h"
#include "tester-utils.h"

#define SIZE (128 * M)
#define ITERS 100000

int main(void) {
    GC_INIT();
    gc_malloc(1);

    int i;
    for (i = 0; i < ITERS; i++) {
        char *a = gc_malloc(SIZE + i);
        if (!a)
            GC_RETURN(1, {;});

        verify_write(a, SIZE);

        int *b = gc_malloc(SIZE + i);
        if (!b)
            GC_RETURN(1, {;});

        verify_write(a, SIZE + i);

        if (!verify_read(a, SIZE))
            GC_RETURN(1, {;});
        if (!verify_read(a, SIZE + i))
            GC_RETURN(1, {;});

        gc_free(a);
        gc_free(b);

        a = gc_malloc(2 * (SIZE + i));
        if (!a)
            GC_RETURN(1, {;});

        verify_write(a, SIZE);
        if (!verify_read(a, SIZE))
            GC_RETURN(1, {;});

        gc_free(a);
    }

    fprintf(stderr, "Memory was allocated, used, and freed!\n");
    GC_RETURN(0, {;});
}
