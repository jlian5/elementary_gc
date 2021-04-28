#include "vector.h"
#include "set.h"

#include <unistd.h>
#include <stdio.h>

// to turn on generations code
// #define USE_GENERATIONS

#ifdef USE_GENERATIONS
//metadata struct
typedef struct gc_metadata {
    int color;
    void *data;
} gc_metadata;

typedef struct generation
{

    // frequency in which we should mark and sweep this generation
    int rate;

    //vector containing the data
    vector *data;

    //amount of data in this gen
    int curr_size;

    //amount of data it should hold before being mark and sweep
    int max_size;

    //next generation
    struct generation *next;

} generation;
#else
typedef struct metaData {
    int isFree;
    size_t size;
    void* ptr;
} metaData;
#endif

extern void **base_stack;
extern void *base_heap;
extern set *in_use;

#ifndef USE_GENERATIONS
#define GC_INIT() \
    do {                                          \
        {base_stack = __builtin_frame_address(0); \
        base_heap = (void*) sbrk(0);              \
        in_use = shallow_set_create();}           \
    } while (0)


#define GC_RETURN(ret_code,callback) \
    do {                                          \
        {vector* v = unused_refs((void*)ret_code);               \
        mark_and_sweep(v);                        \
        vector_destroy(v);                        \
        {callback}                                \
        return (void*)ret_code;}                         \
    } while (0)

#define GC_EXIT(ret_code, callback) \
    do {                                          \
        {vector* v = unused_refs(ret_code);               \
        mark_and_sweep(v);                        \
        vector_destroy(v);                        \
        free_in_use(in_use);                      \
        set_destroy(in_use);                      \
        {callback}                                \
        exit(ret_code);}                          \
    } while (0)
#endif

/**
 * This function should be called immediately before return to see the unused stack references
 * in a function that is about to return. 
 * Then the vector that contains these references can be used to do garbage collecting.
 */
vector *unused_refs(void* ret_val);

/**
 * Will free all malloc'ed memory given the in_use set.
 */
void free_in_use(set *);

#ifdef USE_GENERATIONS
void mark_and_sweep(generation *g);
#else
void mark_and_sweep(vector *v);
#endif

/**
 * Version of malloc for this garbage collector.
 */
void *gc_malloc(size_t);
/**
 * Version of realloc for this garbage collector.
 */
void *gc_realloc(void *, size_t);
/**
 * Version of calloc for this garbage collector.
 */
void *gc_calloc(size_t, size_t);
/**
 * Version of free for this garbage collector. (no-op)
 */
void gc_free(void *);

void add_possible_heap_addr(void* heap_ptr, set* s, void* curr_heap) ;
void scan_possible_heap_addr(void* heap_ptr, set* possible_refs,set* caller_refs, void* curr_heap);
