#include "vector.h"
#include "set.h"
#include "gc.h"

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

#define white 0
#define grey  1
#define black 2

//Used for stack searching
void** base_stack;
void* base_heap;
set* in_use;

#ifdef USE_GENERATIONS
//Generation vectors
//could change this to vectors of generaation vectors
static generation *gen0;
static generation *gen1;
static generation *gen2;

//Amount of times allocation has be called
static int alCount = 0;
static int freeCount = 0;

/**
 * Basic Mark and Sweep algorithm (ruby code below)
 * > while (!grey_slots.empty?)
 * >   current_slot = grey_slots.pop
 * >   grey_slots += current_slot.referenced_slots
 * >   black_slots << current_slot
 * > end
 * This will use the top_level_objects vector and mark (by changing the color)
 * and sweep the objects by freeing them as necessary.
 */
void mark_and_sweep(generation *g)
{
    size_t i, size;
    //the int vector are placeholder, it should be void pointers
    vector *black_slots = int_vector_create();
    vector *grey_slots = int_vector_create();
    vector *white_slots = int_vector_create(); // gen0;

    // MARK:

    vector* stack = unused_refs();
    size = vector_size(stack);

    //mark refrences in stack to grey
    for(i = 0; i < size; i++) {
        gc_metadata *data = vector_get(stack, i);
        data->color = grey;
        vector_push_back(grey_slots, data);
    }

    while (!vector_empty(grey_slots)) {

        gc_metadata *current_slot = (gc_metadata *)vector_back(grey_slots);
        vector_pop_back(grey_slots);

        //go through all the refrences of current_slot, add them to gray_slots if white, and mark them gray

        // VECTOR_FOR_EACH(current_slot->references, referenced, {
        //     if(current_slot->color == 0)
        //         vector_push_back(grey_slots, referenced);
        // }
        
        //bring curr slot to black
        current_slot->color = black;
        vector_push_back(black_slots, current_slot);
    }

    // SWEEP:

    //loop through all white marked data and free it
    size = vector_size(white_slots);
    for(i = 0; i < size; i++) {
        gc_metadata *current_slot = vector_get(white_slots, i);
        if(current_slot->color == 0) {
            free(current_slot);
            freeCount++;
        }
    }

    //loop through all black marked data and move it to the next gen
    size = vector_size(black_slots);
    for(i = 0; i < size; i++) {
        vector_push_back(g->next->data, vector_get(black_slots, i));
    }

    //check to see if mark and sweep should be called on next generation
    check_mark_and_sweep(g->next);

    vector_destroy(black_slots);
    vector_destroy(grey_slots);
    vector_destroy(white_slots);
    vector_destroy(stack);
}
#else
void mark_and_sweep(vector *v) {
    size_t size = vector_size(v);
    //puts("b");
    for(size_t i = 0; i < size; i++) {
        metaData *meta = (void*)vector_get(v, i);
        if(!set_contains(in_use, meta)) {
            continue;
        }
        else if(!(meta->isFree)) {
            // puts("a");
#ifdef DEBUG
            fprintf(stderr, "freed: %p\n", meta);
#endif
            // printf("contained data: %d\n", *(int*)meta->ptr);
            
            free(meta);
        }
    }
}
#endif

#ifdef USE_GENERATIONS
void *gc_malloc(size_t request_size)
{

    //allocate the data
    gc_metadata *ptr = malloc(request_size + sizeof(gc_metadata));

    //return NULL if no data was allocated
    if (!ptr)
        return NULL;

    ptr->color = white;
    ptr->data = (void*)ptr + sizeof(gc_metadata);

    //put the pointer to the allocated data into the gen0 vector
    vector_push_back(gen0, ptr);

    //checks to see if mark_and_sweep() should be called
    check_mark_and_sweep(gen0);

    //return a pointer to the requested data
    return ptr;
}
#else
void *gc_malloc(size_t size) {
    metaData *meta = malloc(sizeof(metaData) + size);
#ifdef DEBUG
    fprintf(stderr, "malloced: %p\n", meta);
#endif
    meta->isFree = 0;
    meta->ptr = (void*)meta + sizeof(metaData);
    set_add(in_use, meta);
    return meta->ptr;
}
#endif

void *gc_calloc(size_t num_elements, size_t element_size) {
    size_t n = num_elements * element_size;
    void *ptr = gc_malloc(n);
    bzero(ptr, n);
    return ptr;
}

#ifdef USE_GENERATIONS
void *gc_realloc(void *ptr, size_t request_size) {
    int i;
    int size = gen0->curr_size;
    for (i = 0; i < size; i++) {
        if (ptr == vector_get(gen0, i)) {
            void *mem = realloc(ptr, request_size);
            vector_set(gen0->data, i, mem);
            check_mark_and_sweep(gen0);
            return mem;
        }
    }
}
#else
void *gc_realloc(void *ptr, size_t request_size) {
    if (ptr == NULL) {
        return gc_malloc(request_size);
    }
    metaData *meta = ptr - sizeof(metaData);
    if(set_contains(in_use, meta) && request_size > 0) {
        set_remove(in_use, meta);
        metaData *new_meta = realloc(meta, sizeof(metaData) + request_size);
#ifdef DEBUG
        fprintf(stderr, "realloced: %p\n", new_meta);
#endif
        new_meta->isFree = 0;
        new_meta->ptr = (void*)new_meta + sizeof(metaData);
        set_add(in_use, new_meta);
        return new_meta->ptr;
    }
    return NULL;
}
#endif

void gc_free(void *ptr)
{
    (void)ptr;
    // no-op by design
}

#ifdef USE_GENERATIONS
//checks whether g should be mark and sweep and call it if it does
void check_mark_and_sweep(generation *g)
{

    // //Method 1
    // malCount++;
    // if (malCount % markSweep == 0)
    // {
    //     mark_and_sweep(g);
    //     malCount = 0;
    // }

    //Method 2
    //calls mark and sweep if total data in gen < current data in gen
    if(g->curr_size > g->max_size) {
        mark_and_sweep(g);
    }

    return;
}
#endif

/**
 * This function should be called immediately before return to see the unused stack references in a function that is about to return. 
 * Then the vector that contains these references can be used to do garbage collecting.
 */
vector* unused_refs() {
    void** caller_stack = __builtin_frame_address(1); //this is the frame of the function that called the function to be cleaned up
    void** curr_stack = __builtin_frame_address(0); //this is the frame of the function that we need to clean
    void* curr_heap = (void*) sbrk(0); //current heap ptr

    set* caller_refs = shallow_set_create(); 

    //scan through the stack frame excluding curr_stack to see all the references that were saved.
    void** ptr = caller_stack; 
    while(ptr < base_stack) {
        if(*ptr >=  base_heap && *ptr < curr_heap) {
            set_add(caller_refs, *ptr);
        }
        ptr++;
    }

    set* unused_refs = shallow_set_create();
    
    //scan through the stack frame to be cleaned up and see if any unsaved refs exist.
    ptr = curr_stack;
    while(ptr < caller_stack) {
        if(*ptr >=  base_heap && *ptr < curr_heap) {
            if(!set_contains(caller_refs, *ptr) && !set_contains(unused_refs, *ptr)) 
                set_add(unused_refs, *ptr);
        }
        ptr++;
    }
    vector* unused_refs_vec = set_elements(unused_refs);
    set_destroy(caller_refs);
    set_destroy(unused_refs);

    return unused_refs_vec; //TODO used this vector to see if any of these unused_refs point to used memory, if so free them.
}

void free_in_use(set *in_use_set) {
#ifdef DEBUG
    fprintf(stderr, "freeing %zu unused references that were found in in_use\n", set_cardinality(in_use_set));
#endif
    SET_FOR_EACH(in_use_set, ptr, {
        (void)ptr;
        // free(ptr - sizeof(metaData));
    });
}
