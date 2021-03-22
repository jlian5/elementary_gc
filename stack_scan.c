#include <stdio.h>
#include <malloc.h>

#include "includes/set.h"

/**
 * gcc -fno-omit-frame-pointer stack_scan.c libs/libset-pthread.a libs/libvector-pthread.a libs/libcompare-pthread.a libs/libcallbacks-pthread.a -o stack_scan
 **/


static void** base_stack;
static void* base_heap;

vector* ret_count_ref();
int f();

int main() {
    base_stack = __builtin_frame_address(0);
    base_heap = sbrk(0);

    int* a[100];
    for(size_t i = 0; i < 100; i ++) a[i] = malloc(sizeof (int));
    f();

    vector* v = ret_count_ref();
    printf("in main %zu\n", vector_size(v));
    vector_destroy(v);

    return;
    
}

int f() {
    int* a = malloc(sizeof(int));


    vector* v = ret_count_ref();
    printf("in f %zu\n", vector_size(v));
    vector_destroy(v);
    return 0;
}

vector* ret_count_ref() {
    void** caller_stack = __builtin_frame_address(1);
    void** curr_stack = __builtin_frame_address(0);
    void* curr_heap = sbrk(0);

    set* caller_refs = shallow_set_create();

    void** ptr = caller_stack;
    while(ptr < base_stack) {
        if(*ptr >=  base_heap && *ptr < curr_heap) {
            set_add(caller_refs, *ptr);
        }
        ptr++;
    }

    vector* unused_refs = shallow_vector_create();
    
    ptr = curr_stack;
    while(ptr < caller_stack) {
        if(*ptr >=  base_heap && *ptr < curr_heap) {
            if(!set_contains(caller_refs, *ptr)) vector_push_back(unused_refs, *ptr);
        }
        ptr++;
    }
    set_destroy(caller_refs);
    return unused_refs; //TODO used this vector to see if any of these unused_refs point to used memory, if so free them.
}