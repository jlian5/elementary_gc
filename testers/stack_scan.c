#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

#include "set.h"
#include "gc.h"
#define _U_ __attribute__((unused))
typedef struct _test_struct{
    size_t mem_size;
    void* ptr_to_heap;
} test_struct;
void* f();
int factorial(int);

int main() {
    GC_INIT();

    // int* array[10];
    // for(size_t i = 0; i < 10; i ++) array[i] = gc_malloc(sizeof (int));
    factorial(3);
    void* _U_ ptr = f();

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
    test1->ptr_to_heap = gc_malloc(4);
    // printf("ptr is %p\n", ptr);
    printf("ptr to heap is %p\n", test1->ptr_to_heap);
    
    // printf("in f %zu\n", vector_size(v));

    // // vector* v = unused_refs();
    
    //printf("a addr: %p\n", a);
    GC_RETURN(0,{puts("end of main");});
}

int factorial(int num) {
    printf("---------------- in factorial() %d-------------------------\n", num);
    gc_malloc(3);
    if (num <= 1) {
        GC_RETURN(num, {printf("---------------- ^^in factorial() %d-------------------------\n", num);});
    }
    int res = num * factorial(num - 1);
    GC_RETURN(res, {printf("---------------- ^^in factorial() %d-------------------------\n", num);});
}

void* f() {
    puts("---------------- in f()-------------------------");
    int* a _U_= gc_malloc(sizeof(int));
    
    // printf("a addr: %p\n", a);
    int* b _U_= gc_malloc(sizeof(int));
    
    // printf("b addr: %p\n", b);
    test_struct *test1 = gc_malloc(sizeof(test_struct));
    test1->mem_size = 4;
    // test1->ptr_to_heap = gc_malloc(4);
    test1->ptr_to_heap = gc_malloc(4);

    int* c _U_= gc_malloc(sizeof(int));
    int* d _U_= gc_malloc(sizeof(int));
    void*p = gc_malloc(2);
    printf("p is %p\n", p);



    GC_RETURN((void*)3,{puts("----------------^^f()^^-------------------------");});
    return 0;
}
