#include "gc.h"
#include "graph.h"

// int is_cyclic(graph *g) {
// 
// }

int factorial(int num) {
    if (num <= 1) {
        GC_RETURN(num, {;});
    }
    GC_RETURN(num * factorial(num - 1), {;});
}

int main(void) {
    GC_INIT();
    fprintf(stdout, "%d\n", factorial(10));
    GC_RETURN(0, {puts("done!");});
}
