#include "vector.h"
#include "set.h"
#include "gc.h"

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

//Used for stack searching
void** base_stack;
void* base_heap;
set* in_use;

//Generation vectors (could change this to vectors of generation vectors)
vector *gen1;
vector *gen2;
vector *boomers; //gen3

//total alloc calls
int allocTotal = 0;

//alloc limits for each gen
int limit0 = 4;
int limit1 = 16;
int limit2 = 64;
int ok_boomer = 256; //limit 3

void mark_and_sweep(vector *v) {
    size_t size = vector_size(v);
    //puts("b");
    for(size_t i = 0; i < size; i++) {
        metaData *meta = (void*)vector_get(v, i) - sizeof(metaData);
        if(!set_contains(in_use, meta->ptr)) {
            continue;
        }
        else if(!(meta->isFree)) {
#ifdef DEBUG
            fprintf(stderr, "freed: %p\n", meta->ptr);
#endif
            // printf("contained data: %d\n", *(int*)meta->ptr);
            set_remove(in_use, meta->ptr);
            free(meta);
        }
    }
}

// void mark_and_sweep(vector *v, vector *n) {
//     size_t size = vector_size(v);
//     for(size_t i = 0; i < size; i++) {
//         metaData *meta = (void*)vector_get(v, i) - sizeof(metaData);
//         if(!set_contains(in_use, meta->ptr)) {
//             if (n != NULL) {
//                 vector_push_back(n, meta);
//                 //set_remove(in_use, meta);
//                     fprintf(stderr, "moved: %p\n", meta);//
//             }
//             else {
//                 continue;
//             }
//         }
//         else if(!(meta->isFree)) {
//                 fprintf(stderr, "freed: %p\n", meta->ptr);//
//             // printf("contained data: %d\n", *(int*)meta->ptr);
//             meta->isFree = 1;
//             set_remove(in_use, meta->ptr);
//             free(meta);
//         }
//     }
// }

void *gc_malloc(size_t size) {
    metaData *meta = malloc(sizeof(metaData) + size);
    meta->isFree = 0;
    meta->size = size;
    meta->ptr = (void*)meta + sizeof(metaData);
    #ifdef DEBUG
    fprintf(stderr, "malloced: %p\n", meta->ptr);
    #endif
    set_add(in_use, meta->ptr);
    //call_mark_and_sweep();    
    return meta->ptr;
}

void gc_exit() {
    vector* v = unused_refs(NULL);
    mark_and_sweep(v);
    vector_destroy(v);
    set_destroy(in_use);
}

void *gc_calloc(size_t num_elements, size_t element_size) {
    size_t n = num_elements * element_size;
    void *ptr = gc_malloc(n);
    bzero(ptr, n);
    return ptr;
}

void *gc_realloc(void *ptr, size_t request_size) {
    if (ptr == NULL) {
        return gc_malloc(request_size);
    }
    metaData *meta = ptr - sizeof(metaData);
    if(set_contains(in_use, meta->ptr) && request_size > 0) {
        set_remove(in_use, meta->ptr);
        metaData *new_meta = realloc(meta, sizeof(metaData) + request_size);
        #ifdef DEBUG
        fprintf(stderr, "realloced: %p\n", new_meta);
        #endif
        new_meta->isFree = 0;
        new_meta->size = request_size;
        new_meta->ptr = (void*)new_meta + sizeof(metaData);
        set_add(in_use, new_meta->ptr);
        //call_mark_and_sweep();
        return new_meta->ptr;
    }
    return NULL;
}

void gc_free(void *ptr)
{
    (void)ptr;
    // no-op by design
}

//checks whether g should be mark and sweep and call it if it does
// void call_mark_and_sweep()
// {

//     //Method 1
//     allocTotal++;
//     if(allocTotal % limit0 == 0)
//         mark_and_sweep(unused_refs(NULL), gen1); //function for getting refs in stack frame
//     if(allocTotal % limit1 == 0)
//         mark_and_sweep(gen1, gen2);
//     if(allocTotal % limit2 == 0)
//         mark_and_sweep(gen2, boomers);
//     if(allocTotal % ok_boomer == 0) {
//         mark_and_sweep(boomers, NULL);
//         allocTotal = 0;
//     }

//     //Method 2
//     //calls mark and sweep if total data in gen < current data in gen
//     // if(g->curr_size > g->max_size) {
//     //     mark_and_sweep(g);
//     // }
//     return;
// }

/**
 * This function should be called immediately before return to see the unused stack references in a function that is about to return. 
 * Then the vector that contains these references can be used to do garbage collecting.
 */
vector* unused_refs(void* ret_val) {
    void** caller_stack = __builtin_frame_address(1); //this is the frame of the function that called the function to be cleaned up
    void* curr_heap = (void*) sbrk(0); //current heap ptr

    set* caller_refs = shallow_set_create(); 
    // if(ret_val >= base_heap && ret_val < curr_heap){
    if(set_contains(in_use, ret_val)){
        add_possible_heap_addr(ret_val, caller_refs, curr_heap);
    }

    //scan through the stack frame excluding curr_stack to see all the references that were saved.
    void** ptr = caller_stack; 
    while(ptr < base_stack) {
        // printf("*ptr is %p\n", *ptr);
        if(*ptr >=  base_heap && *ptr < curr_heap) {
            add_possible_heap_addr(*ptr, caller_refs, curr_heap);
        }
        ptr++;
    }
    
    set* unused_refs = shallow_set_create();
    
    vector* inuse_vec = set_elements(in_use);
    VECTOR_FOR_EACH(inuse_vec, i, {
        if(!set_contains(caller_refs, i)) set_add(unused_refs, i);
    });
    
    vector* unused_refs_vec = set_elements(unused_refs);
    set_destroy(unused_refs);
    set_destroy(caller_refs);

    return unused_refs_vec; 
}

//add all suspicious ptrs in the memory region(on the heap) allocated by heap_ptr into the set
void add_possible_heap_addr(void* heap_ptr, set* s, void* curr_heap) {
    if(!set_contains(in_use, heap_ptr) || set_contains(s, heap_ptr)) return;
    
    set_add(s, heap_ptr);

    metaData *meta = heap_ptr - sizeof(metaData);
    size_t scanning_size = meta->size; 
    printf("%zu\n", scanning_size);

    
    for(size_t i = 0; i < scanning_size; i++) {
        void* ptr = *(void**) (heap_ptr+i);
        if(ptr >=  base_heap && ptr< curr_heap){
            
            add_possible_heap_addr(ptr, s, curr_heap);
        }
    }

}

//same as add_possible_heap_addr but this time it will add if it is not in caller_refs
void scan_possible_heap_addr(void* heap_ptr, set* possible_refs,set* caller_refs, void* curr_heap) {
    printf("heap ptr is %p\n", heap_ptr);
    if(!set_contains(in_use, heap_ptr)) return;
    if(!set_contains(caller_refs, heap_ptr) && !set_contains(possible_refs, heap_ptr)) set_add(possible_refs, heap_ptr);
    metaData *meta = heap_ptr - sizeof(metaData);
    size_t scanning_size = meta->size;
    // printf("size is %zu\n", scanning_size);
    for(size_t i = 0; i < scanning_size; i++) {
        void* ptr = *(void**) (heap_ptr+i);
        // printf("loop ptr is %p\n", ptr);
        if(ptr >=  base_heap && ptr < curr_heap){
            scan_possible_heap_addr(ptr, possible_refs, caller_refs,curr_heap);
        }
    }

}

