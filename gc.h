#pragma once

#include "vector.h"
#include "set.h"

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>


typedef struct metaData {
    int isFree;
    size_t size;
    void* ptr;
} metaData;

extern void **base_stack;
extern void *base_heap;
extern set *in_use;

extern vector *gen1;
extern vector *gen2;
extern vector *boomers; //gen3

extern int allocTotal;
extern int limit0;
extern int limit1;
extern int limit2;
extern int ok_boomer; //limit 3

#define GC_INIT() \
    do {                                          \
        {base_stack = __builtin_frame_address(0); \
        base_heap = (void*) sbrk(0);              \
        in_use = shallow_set_create();            \
        gen1 = shallow_vector_create();           \
        gen2 = shallow_vector_create();           \
        boomers = shallow_vector_create();        \
        atexit(gc_exit);}                         \
    } while (0)


#define GC_RETURN(ret_code,callback) \
    do {                                          \
        {vector* v = unused_refs((void*)(uintptr_t)(ret_code));               \
        mark_and_sweep(v);                        \
        vector_destroy(v);                        \
        {callback}                                \
        return (ret_code);}                         \
    } while (0)

#define GC_EXIT(ret_code, callback) \
    do {                                          \
        {vector* v = unused_refs((void*)(uintptr_t)(ret_code));               \
        mark_and_sweep(v);                        \
        vector_destroy(v);                        \
        set_destroy(in_use);                      \
        vector_destroy(gen1);                     \
        vector_destroy(gen2);                     \
        vector_destroy(boomers);                  \
        {callback}                                \
        exit(ret_code);}                          \
    } while (0)

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

void mark_and_sweep(vector *v);

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

/**
 * Function to be called at every exit. Will clean up the rest of the malloc'ed pointers
 */
void gc_exit();

void add_possible_heap_addr(void* heap_ptr, set* s, void* curr_heap) ;
void scan_possible_heap_addr(void* heap_ptr, set* possible_refs,set* caller_refs, void* curr_heap);
