#include "gc.h"

#include <stdio.h>

//Test program for gc
int main(void) {
    GC_INIT();
    char *ptr1 = gc_malloc(4);
    strncpy(ptr1, "abc", 4);
    puts(ptr1); 
    GC_EXIT(0, {puts("done");});
}
