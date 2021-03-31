#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

#include "../includes/set.h"
#include "../gc.h"

int f();

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

