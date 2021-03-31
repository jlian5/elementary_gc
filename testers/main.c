#include "gc.h"

#include <stdio.h>

//Test program for gc
int main(){
    char *ptr1 = gc_malloc(8);
    ptr1 = "abc";
    puts(ptr1); 
    return 0;
}