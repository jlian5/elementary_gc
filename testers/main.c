#include "gc.h"

#include <stdio.h>

//Test program for gc
int main(void) {
    char *ptr1 = gc_malloc(8);
    ptr1 = "abc";
    puts(ptr1); 
    GC_RETURN(0, {puts("done");});
}
