#include <stdio.h>
#include <malloc.h>

#include "includes/set.h"
#include <unistd.h>
/**
 * gcc -fno-omit-frame-pointer stack_scan.c libs/libset-pthread.a libs/libvector-pthread.a libs/libcompare-pthread.a libs/libcallbacks-pthread.a -o stack_scan
 **/

#define GC_INIT() \
    do {                                            \
        {base_stack = __builtin_frame_address(0);  \
        base_heap = (void*) sbrk(0);              \
        in_use = shallow_set_create();}            \
    } while (0)


#define GC_RETURN(ret_code,callback) \
    do {                                            \
        {vector* v = unused_refs();                \
        mark_and_sweep(v);                        \
        vector_destroy(v);                        \
        {callback}                                  \
        return ret_code;}                           \
    } while (0)

#define GC_EXIT(ret_code, callback) \
    do {                                            \
        {vector* v = unused_refs();                \
        mark_and_sweep(v);                        \
        vector_destroy(v);                        \
        set_destroy(in_use);                      \
        {callback}                                  \
        exit(ret_code);}                           \
    } while (0)


typedef struct metaData {
    int isFree;
    char ptr[0];
} metaData;

static void** base_stack;
static void* base_heap;
static set* in_use;

vector* unused_refs();
int f();
void *gc_malloc();
void mark_and_sweep();



int main() {
    GC_INIT();

    int* array[10];
    for(size_t i = 0; i < 10; i ++) array[i] = gc_malloc(sizeof (int));

    f();

    puts("---------------- in main()-------------------------");

    puts("\nbelow is variables");
    int* a = gc_malloc(sizeof(int));
    printf("%p\n", a);
    // printf("a addr: %p\n", a);
    //free(a);
    int* b = gc_malloc(sizeof(int));
    *b = 2;
    printf("%p\n", b);
    //printf("b addr: %p\n", b);

    int* c = gc_malloc(sizeof(int));
    *c = 3;
    printf("%p\n", c);
    //free(b);
    int* d = gc_malloc(sizeof(int));
    *d = 4;
    printf("%p\n", d);


    
    // printf("in f %zu\n", vector_size(v));

    // // vector* v = unused_refs();
    
    //printf("a addr: %p\n", a);
    GC_EXIT(0,{puts("end of main");});
    return 0;
}

int f() {
    puts("---------------- in f()-------------------------");
    int* a = gc_malloc(sizeof(int));
    // printf("a addr: %p\n", a);
    int* b = gc_malloc(sizeof(int));
    // printf("b addr: %p\n", b);

    int* c = gc_malloc(sizeof(int));
    int* d = gc_malloc(sizeof(int));



    GC_RETURN(0,{puts("----------------^^f()^^-------------------------");});
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
    // meta->ptr = (void *)meta + sizeof(metaData);
    set_add(in_use, meta->ptr);
    return meta->ptr;
}

void mark_and_sweep(vector *v) {
    size_t size = vector_size(v);
    //puts("b");
    for(size_t i = 0; i < size; i++) {
        if(!set_contains(in_use, vector_get(v, i))) continue;
        metaData *meta = (void*)vector_get(v, i) - sizeof(metaData);
        if(!(meta->isFree)){
            // puts("a");
            printf("freed: %p\n", meta);
            // printf("contained data: %d\n", *(int*)meta->ptr);
            
            free(meta);
        }
    }
}
