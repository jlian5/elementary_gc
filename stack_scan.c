#include <stdio.h>
#include <malloc.h>

static void** base_stack;
static void* base_heap;

size_t count_ref();

int main() {
    base_stack = __builtin_frame_address(0);
    base_heap = sbrk(0);
    int *a[1024];
    for(size_t i = 0; i < 1024; i++) a[i] = malloc(sizeof(int));
    printf("a: %p, base_heap: %p\n", a, base_heap);
    printf("%zu\n", count_ref());
    
    for(size_t i = 0; i < 1024; i++)free(a[i]);

    printf("%zu", count_ref());
}

size_t count_ref() {
    void** curr_stack = __builtin_frame_address(0);
    void* curr_heap = sbrk(0);

    size_t cnt = 0;
    void** ptr = curr_stack;
    // printf("current heap: %p\n", curr_heap);
    while(ptr < base_stack) {
        if(*ptr >=  base_heap && *ptr < curr_heap) cnt++;
        ptr++;
        // printf("current ptr: %p\n", ptr);
    }
    return cnt;
}