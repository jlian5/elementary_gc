#include "gc.h"

void f() {
    printf("in f: %p\n", __builtin_frame_address (0));
}

int main(int argc, char** argv) {
    // TODO: put testing code here
    int a,b;
    printf("%x\n", *(&argc + 1));
    printf("%p\n", __builtin_frame_address (0));
    printf("argc addr: %p, argv addr: %p, a addr: %p, b addr: %p\n", &argc,&argv, &a, &b);
    printf("%p\n", __builtin_frame_address (0));
    f();
    return 0;
}

