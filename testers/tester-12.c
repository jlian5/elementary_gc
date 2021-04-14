/**
 * malloc
 * CS 241 - Spring 2021
 */
#include "gc.h"
#include "tester-utils.h"

#define SIZE (1024L * M)
#define ITERS 10

int main(void) {
    GC_INIT();
    gc_malloc(1);
    int i;
    for (i = 0; i < ITERS; i++) {
        // Write to end
        char *a = calloc(SIZE, sizeof(char));
        if (!a)
            GC_RETURN(1, {;});

        verify_clean(a, SIZE);
        verify_write(a, SIZE);
        if (!verify_read(a, SIZE))
            GC_RETURN(1, {;});

        gc_free(a);

        char *b = calloc(SIZE / 2, sizeof(char));
        verify_clean(b, SIZE / 2);
        verify_write(b, SIZE / 2);

        char *c = calloc(SIZE / 4, sizeof(char));
        verify_clean(c, SIZE / 4);
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
