/**
 * malloc
 * CS 241 - Spring 2021
 */
#include "gc.h"
#include "tester-utils.h"

#define MIN_ALLOC_SIZE (256 * K * K)
#define MAX_ALLOC_SIZE (K * K * K)

void *malloc_and_break(void *region, int c, size_t len) {
    if (len < MIN_ALLOC_SIZE) {
        GC_RETURN(region, {;});
    }

    void *sr1 = gc_realloc(region, len / 3);
    void *sr2 = gc_malloc(len / 3);
    void *sr3 = gc_malloc(len / 3);

    verify_overlap3(sr1, sr2, sr3, len / 3);
    verify(sr1, c, len / 3);

    memset(sr1, 0xab, len / 3);
    memset(sr2, 0xcd, len / 3);
    memset(sr3, 0xef, len / 3);
    gc_free(sr2);

    sr1 = malloc_and_break(sr1, 0xab, len / 3);
    sr3 = malloc_and_break(sr3, 0xef, len / 3);

    sr1 = gc_realloc(sr1, len / 2);
    sr3 = gc_realloc(sr3, len / 2);

    verify(sr1, 0xab, len / 3);
    verify(sr3, 0xef, len / 3);

    memset(sr1, 0xab, len / 2);
    memset(sr3, 0xef, len / 2);

    verify_overlap2(sr1, sr3, len / 2);
    gc_free(sr3);

    sr1 = gc_realloc(sr1, len);
    verify(sr1, 0xab, len / 2);

    memset(sr1, c, len);

    GC_RETURN(sr1, {;});
}

int main(void) {
    GC_INIT();
    gc_malloc(1);

    size_t len = MAX_ALLOC_SIZE;
    while (len > MIN_ALLOC_SIZE) {
        void *mem = gc_malloc(len);
        memset(mem, 0xff, len);
        gc_free(malloc_and_break(mem, 0xff, len));
        len /= 3;
    }

    fprintf(stderr, "Memory was allocated and freed!\n");
    GC_RETURN(0, {;});
}
