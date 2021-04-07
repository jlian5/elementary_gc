#include "vector.h"
#include "set.h"

#include <unistd.h>
#include <stdio.h>

typedef struct metaData {
    int isFree;
    char ptr[0];
} metaData;


extern void** base_stack;
extern void* base_heap;
extern set* in_use;

#ifndef USE_MARK_SWEEP
#define GC_INIT() \
    do {                                          \
        {base_stack = __builtin_frame_address(0); \
        base_heap = (void*) sbrk(0);              \
        in_use = shallow_set_create();}           \
    } while (0)


#define GC_RETURN(ret_code,callback) \
    do {                                          \
        {vector* v = unused_refs();               \
        mark_and_sweep(v);                        \
        vector_destroy(v);                        \
        {callback}                                \
        return ret_code;}                         \
    } while (0)

#define GC_EXIT(ret_code, callback) \
    do {                                          \
        {vector* v = unused_refs();               \
        mark_and_sweep(v);                        \
        vector_destroy(v);                        \
        set_destroy(in_use);                      \
        {callback}                                \
        exit(ret_code);}                          \
    } while (0)
#endif

/**
 * This function should be called immediately before return to see the unused stack references in a function that is about to return. 
 * Then the vector that contains these references can be used to do garbage collecting.
 **/
vector* unused_refs();

#ifdef USE_MARK_SWEEP
void mark_and_sweep(generation *g);
#else
void mark_and_sweep(vector *v);
#endif

void *gc_malloc(size_t);
void *gc_realloc(void *, size_t);
void *gc_calloc(size_t, size_t);
void gc_free(void *);
