#include <stdio.h>
#include <malloc.h>

#include "includes/set.h"
#include <unistd.h>
/**
 * gcc -fno-omit-frame-pointer stack_scan.c libs/libset-pthread.a libs/libvector-pthread.a libs/libcompare-pthread.a libs/libcallbacks-pthread.a -o stack_scan
 **/

typedef struct metaData {
    int isFree;
    void *ptr;
} metaData;

static void** base_stack;
static void* base_heap;

vector* unused_refs();
int f();
void *gc_malloc();
void mark_and_sweep();

int main() {
    base_stack = __builtin_frame_address(0);
    base_heap = (void*) sbrk(0);

    // //int* a[100];
    // //for(size_t i = 0; i < 100; i ++) a[i] = gc_malloc(sizeof (int));
    // //f();

    int* a = gc_malloc(sizeof(int));
    printf("a addr: %p\n", a);
    //free(a);
    int* b = gc_malloc(sizeof(int));
    *b = 2;
    //printf("b addr: %p\n", b);

    int* c = gc_malloc(sizeof(int));
    *c = 3;
    //free(b);
    int* d = gc_malloc(sizeof(int));
    *d = 4;


    vector* v = unused_refs();
    printf("in f %zu\n", vector_size(v));

    // // vector* v = unused_refs();
    mark_and_sweep(v);
    //printf("a addr: %p\n", a);
    // // printf("in main %zu\n", vector_size(v));
    vector_destroy(v);

    return 0;
}

int f() {
    int* a = malloc(sizeof(int));
    printf("a addr: %p\n", a);
    free(a);
    int* b = malloc(sizeof(int));
    printf("b addr: %p\n", b);

    int* c = malloc(sizeof(int));
    free(b);
    int* d = malloc(sizeof(int));


    vector* v = unused_refs();
    printf("in f %zu\n", vector_size(v));
    vector_destroy(v);
    return 0;
}


/**
 * This function should be called immediately before return to see the unused stack references in a function that is about to return. 
 * Then the vector that contains these references can be used to do garbage collecting.
 **/
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

void *gc_malloc(size_t size) {
    metaData *meta = malloc(sizeof(metaData) + size);
    printf("malloced: %p\n", meta);
    meta->isFree = 0;
    meta->ptr = (void *)meta + sizeof(metaData);
    return meta->ptr;
}

void mark_and_sweep(vector *v) {
    size_t size = vector_size(v);
    //puts("b");
    for(size_t i = 0; i < size; i++) {
        metaData *meta = (void*)vector_get(v, i) - sizeof(metaData);
        if(!(meta->isFree)){
            puts("a");
            printf("freed: %p\n", meta);
            free(meta);
        }
    }
}