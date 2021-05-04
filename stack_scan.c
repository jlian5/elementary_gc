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


typedef struct _metaData {
    int isFree;
    size_t size;
    char ptr[0];
} metaData;

typedef struct _test_struct{
    size_t mem_size;
    void* ptr_to_heap;
} test_struct;

static void** base_stack;
static void* base_heap;
static set* in_use;

vector* unused_refs();
int f(void**);
void *gc_malloc();
void mark_and_sweep();
void add_possible_heap_addr(void* heap_ptr, set* s, void* curr_heap) ;
void scan_possible_heap_addr(void* heap_ptr, set* possible_refs,set* caller_refs, void* curr_heap);


int main() {
    GC_INIT();

    // int* array[10];
    // for(size_t i = 0; i < 10; i ++) array[i] = gc_malloc(sizeof (int));
    void* ptr;
    f(&ptr);

    puts("---------------- in main()-------------------------");

    // puts("\nbelow is variables");
    // int* a = gc_malloc(sizeof(int));

    // int* b = gc_malloc(sizeof(int));
    // *b = 2;



    // int* c = gc_malloc(sizeof(int));
    // *c = 3;


    // int* d = gc_malloc(sizeof(int));
    // *d = 4;

    test_struct *test1 = gc_malloc(sizeof(test_struct));
    test1->mem_size = 4;
    test1->ptr_to_heap = gc_malloc(4);
    // printf("ptr is %p\n", ptr);
    printf("ptr to heap is %p\n", test1->ptr_to_heap);
    
    // printf("in f %zu\n", vector_size(v));

    // // vector* v = unused_refs();
    
    //printf("a addr: %p\n", a);
    GC_EXIT(0,{puts("end of main");});
    return 0;

}

int f(void** p) {
    puts("---------------- in f()-------------------------");
    int* a = gc_malloc(sizeof(int));
    // printf("a addr: %p\n", a);
    int* b = gc_malloc(sizeof(int));
    // printf("b addr: %p\n", b);

    int* c = gc_malloc(sizeof(int));
    int* d = gc_malloc(sizeof(int));
    *p = gc_malloc(2);
    printf("p is %p\n", *p);



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
        // printf("*ptr is %p\n", *ptr);
        if(*ptr >=  base_heap && *ptr < curr_heap) {
            add_possible_heap_addr(*ptr, caller_refs, curr_heap);
        }
        ptr++;
    }
    
    set* unused_refs = shallow_set_create();
    
    //scan through the stack frame to be cleaned up and see if any unsaved refs exist.
    // ptr = curr_stack;
    // while(ptr < caller_stack) {
    //     if(*ptr >=  base_heap && *ptr < curr_heap) {
    //         scan_possible_heap_addr(*ptr, unused_refs, caller_refs, curr_heap);
    //     }
    //     ptr++;
    // }
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

void *gc_malloc(size_t size) {
    metaData *meta = malloc(sizeof(metaData) + size);
    printf("malloced data: %p\n", meta->ptr);
    // printf("malloced meta: %p\n", meta);
    meta->isFree = 0;
    meta->size = size;
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
            printf("freed: %p\n", meta->ptr);
            // printf("contained data: %d\n", *(int*)meta->ptr);
            set_remove(in_use, meta->ptr);
            free(meta);
        }
    }
}
