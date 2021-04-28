/**
 * malloc
 * CS 241 - Spring 2021
 */
#include "gc.h"
#include "tester-utils.h"

#define SIZE (2L * 1024L * M)
#define ITERS 10000

int main(void) {
    GC_INIT();
    gc_malloc(1);
    int i;
    for (i = 0; i < ITERS; i++) {
        // Write to end

        srand(rand_today());
        int r = rand() % 10;
        char *a = gc_malloc(SIZE + r);

        if (!a)
            GC_RETURN(1, {;});

        verify_write(a, SIZE);
        if (!verify_read(a, SIZE))
            GC_RETURN(1, {;});

        gc_free(a);

        char *b = gc_malloc(SIZE / 2);
        verify_write(b, SIZE / 2);

        char *c = gc_malloc(SIZE / 4);
        verify_write(c, SIZE / 4);

        if (!b || !c)
            GC_RETURN(1, {;});

        if (!verify_read(b, SIZE / 2) || !verify_read(c, SIZE / 4) ||
            overlap(b, SIZE / 2, c, SIZE / 4))
            GC_RETURN(1, {;});

        gc_free(b);
        gc_free(c);
    }

    fprintf(stderr, "Memory was allocated, used, and freed!\n");
    GC_RETURN(0, {;});
}
