/**
 * malloc
 * CS 241 - Spring 2021
 */
#include "gc.h"
#include "tester-utils.h"

#define MIN_ALLOC_SIZE 24
#define MAX_ALLOC_SIZE 1024 * 100
#define CHANCE_OF_FREE 30
#define CHANCE_OF_REALLOC 80
#define TOTAL_ALLOCS 90000

int main(void) {
    GC_INIT();
    srand(rand_today());
    gc_malloc(1);

    int i;
    void *realloc_ptr = NULL;
    void **dictionary = gc_malloc(TOTAL_ALLOCS * sizeof(void *));
    int *dictionary_elem_size = gc_malloc(TOTAL_ALLOCS * sizeof(int));
    int dictionary_ct = 0;
    int data_written = 0;

    for (i = 0; i < TOTAL_ALLOCS; i++) {
        int size =
            (rand() % (MAX_ALLOC_SIZE - MIN_ALLOC_SIZE + 1)) + MIN_ALLOC_SIZE;
        void *ptr;

        if (realloc_ptr == NULL) {
            ptr = gc_malloc(size);
            gc_malloc(1);
            data_written = 0;
        } else {
            ptr = gc_realloc(realloc_ptr, size);
            gc_malloc(1);
            realloc_ptr = NULL;
        }

        if (ptr == NULL) {
            fprintf(stderr, "Memory failed to allocate!\n");
            GC_RETURN(1, {;});
        }

        if (rand() % 100 < CHANCE_OF_FREE)
            gc_free(ptr);
        else {
            if (!data_written) {
                *((void **)ptr) = &dictionary[dictionary_ct];
                data_written = 1;
            }

            if (rand() % 100 < CHANCE_OF_REALLOC)
                realloc_ptr = ptr;
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
            fprintf(
                stderr,
                "Memory failed to contain correct data after many allocations "
                "(beginning of segment)!\n");
            GC_RETURN(100, {;});
        }

        if (*((void **)(dictionary[i] + dictionary_elem_size[i] -
                        sizeof(void *))) != &dictionary[i]) {
            fprintf(
                stderr,
                "Memory failed to contain correct data after many allocations "
                "(end of segment)!\n");
            GC_RETURN(101, {;});
        }

        /*
        char *memory_check = dictionary[i] + sizeof(void *);
        char *memory_check_end = dictionary[i] + dictionary_elem_size[i] -
        sizeof(void *) - 1;

        while (memory_check > memory_check_end)
        {
                if (*memory_check != 0x00)
                {
                        fprintf(stderr, "Memory failed to contain correct data
        after many
        allocations (mid segment or reused segment)!\n");
                        return 102;
                }

                *memory_check = 0x01;
                memory_check++;
        }
        */
        gc_free(dictionary[i]);
    }

    fprintf(stderr, "Memory was allocated and freed!\n");
    GC_RETURN(0, {;});
}
