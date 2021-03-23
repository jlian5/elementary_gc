#include "gc.h"
#include "vector.h"

typedef struct gc_metadata {

    /**
     * Whether this object is:
     * - white = 0
     * - grey = 1
     * - black = 2
     */
    int color;

    //pointer to data
    void *data;

} gc_metadata;

typedef struct generation
{

    // frequency in which we should mark and sweep this generation
    int rate;

    //vector containing the data
    vector *data;

    //amount of data in this gen
    int curr_size;

    //amount of data it should hold before being mark and sweep
    int max_size;

    //next generation
    generation *next;

} generation;

//Generation structs
static generation *gen0;
static generation *gen1;
static generation *gen2;

//place holder for stack memory
static vector *root;

/**
 * malCount = amount of times malloc, calloc, and realloc has been called
 * markSweep = amount of malloc calls required for a mark and sweep call
 */
static int malCount = 0;
static const int markSweep = 4;

/**
 * Basic Mark and Sweep algorithm (ruby code below)
 * > while (!grey_slots.empty?)
 * >   current_slot = grey_slots.pop
 * >   grey_slots += current_slot.referenced_slots
 * >   black_slots << current_slot
 * > end
 * This will use the top_level_objects vector and mark (by changing the color)
 * and sweep the objects by freeing them as necessary.
 */
void mark_and_sweep(generation *g)
{
    int i, size;
    //the int vector are placeholder, it should be void pointers
    vector *black_slots = int_vector_create();
    vector *grey_slots = int_vector_create();
    vector *white_slots = int_vector_create();

    // MARK:

    //This section for marking gray objects from the stack, may not be needed
    size = vector_size(root);
    //mark all the refrences in the stack to gray and add them to grey_slots if they are marked white
    for(i = 0; i < size; i++) {
        //check to make sure heap data is from the specified gen g
    }

    while (!vector_empty(grey_slots))
    {
        gc_metadata *current_slot = (gc_metadata *)vector_back(grey_slots);
        vector_pop_back(grey_slots);

        //go through all the refrences of current_slot, add them to gray_slots if white, and mark them gray

        // VECTOR_FOR_EACH(current_slot->references, referenced, {
        //     vector_push_back(grey_slots, referenced);
        // });
        
        //bring curr slot to black
        vector_push_back(black_slots, current_slot);
    }

    // SWEEP:

    //loop through all white marked data and free it
    size = vector_size(white_slots);
    for(i = 0; i < size; i++) {
        free(vector_get(white_slots, i));
    }

    //loop through all black marked data and move it to the next gen
    size = vector_size(black_slots);
    for(i = 0; i < size; i++) {
        vector_push_back(g->next->data, vector_get(black_slots, i));
    }

    //check to see if mark and sweep should be called on next generation
    check_mark_and_sweep(g->next);

    vector_destory(black_slots);
    vector_destory(grey_slots);
    vector_destroy(white_slots);
}

void *gc_malloc(size_t request_size)
{

    //allocate the data
    gc_metadata *ptr = malloc(request_size + sizeof(gc_metadata));

    //return NULL if no data was allocated
    if (!ptr)
        return NULL;

    ptr->color = 0;
    ptr->data = (void*)ptr + sizeof(gc_metadata);

    //put the pointer to the allocated data into the gen0 vector
    vector_push_back(gen0, ptr);

    //checks to see if mark_and_sweep() should be called
    check_mark_and_sweep(gen0);

    //return a pointer to the requested data
    return ptr;
}

void *gc_calloc(size_t num_elements, size_t element_size)
{
    size_t n = num_elements * element_size;
    void *ptr = gc_malloc(num_elements * element_size);
    bzero(ptr, n);
    return ptr;
}

//double check this works
void *gc_realloc(void *ptr, size_t request_size)
{

    if (ptr == NULL)
    {
        return gc_malloc(request_size);
    }

    int i;
    int size = gen0->curr_size;
    for (i = 0; i < size; i++)
    {
        if (ptr == vector_get(gen0, i))
        {

            void *mem = realloc(ptr, request_size);
            vector_set(gen0->data, i, mem);

            check_mark_and_sweep(gen0);

            return mem;
        }
    }

    return NULL;
}

void gc_free(void *ptr)
{
    // no-op by design
}

//checks whether g should be mark and sweep and call it if it does
void check_mark_and_sweep(generation *g)
{

    // //Method 1
    // malCount++;
    // if (malCount % markSweep == 0)
    // {
    //     mark_and_sweep(g);
    //     malCount = 0;
    // }

    //Method 2
    //calls mark and sweep if total data in gen < current data in gen
    if(g->curr_size > g->max_size) {
        mark_and_sweep(g);
    }

    return;
}

// void malloc(size_t size) {
//     return sbrk(size);
// }

// void calloc(size_t num_elements, size_t element_size) {
//     return sbrk(num_elements * element_size);
// }

// void realloc(void *ptr, size_t size) {
//     return sbrk(size);
// }