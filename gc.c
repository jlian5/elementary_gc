#include "gc.h"
#include "vector.h"

/**
 * Metadata for all garbage collection objects
 */
typedef struct gc_metadata {
    /**
     * Whether this object is:
     * - black(0): referenced, found from root
     * - grey(0): reachable from root
     * - white(0): unreachable -> garbage collect
     */
    int color;

    /**
     * List of other garbage collector objects that can be referenced
     * This will be a vector with struct _gc_metadata objects inside
     * (created using shallow_vector_create() in vector.h)
     */
    //vector *references;

    // /**
    //  * These are vectors for each generation
    //  * Each vector has some data associated to describe the vector
    //  */
    
    struct generation *gen0;
    struct generation *gen1;
    struct generation *gen2;

    // /**
    //  * Sets the data capacity for each vector
    //  */
    // int capacity = 2048;
    // gen0->max_data = capacity;
    // gen1->max_data = capacity/2;
    // gen2->max_data = capacity/4;

} gc_metadata;

typedef struct generation {

    // //max amount of data for a vector until it needs to be mark and sweep
    // int max_data;
    
    // frequency in which we should mark and sweep this generation
    int rate;

    //vector containing the data
    struct vector *data;

} generation;

/**
 * Contains a list of the top level objects that are accessible.
 * TODO: change this to add different vectors for different aged objects
 */
static struct vector *top_level_objects;
static struct vector *grey_slots;
static struct vector *black_slots;

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
void mark_and_sweep() {
    // MARK:
    while (!vector_empty(grey_slots)) {
        gc_metadata_t *current_slot = (gc_metadata_t *) vector_back(grey_slots);
        vector_pop_back(grey_slots);
        VECTOR_FOR_EACH(current_slot->references, referenced, {
            vector_push_back(grey_slots, referenced);
        });
        vector_push_back(black_slots, current_slot);
    }

    // SWEEP:
    // free all white slots (nonreachable from grey slots)
}

//contains all generations
static gc_metadata gc;


void *gc_malloc(size_t request_size) {

    //allocate the data
    void *ptr = malloc(request_size);

    //return NULL if no data was allocated
    if (!ptr) return NULL;

    //put the pointer to the allocated data into the gen0 vector
    vector_push_back(gc->gen0, ptr);

    //checks to see if mark_and_sweep() should be called
    malCount++;
    if(malCount % markSweep == 0) {
        mark_and_sweep()
        malCount = 0;
    }

    //return a pointer to the requested data
    return ptr;
}

void *gc_calloc(size_t num_elements, size_t element_size) {

    //allocate the data
    void *ptr = calloc(num_elements, element_size);

    //return NULL if no data was allocated
    if (!ptr) return NULL;

    //put the pointer to the allocated data into the gen0 vector
    vector_push_back(gc->gen0, ptr);

    //checks to see if mark_and_sweep() should be called
    malCount++;
    if(malCount % markSweep == 0) {
        mark_and_sweep()
        malCount = 0;
    }

    //return a pointer to the requested data
    return ptr;
}

void *gc_realloc(void *ptr, size_t request_size) {

    if (ptr == NULL)
    {
        return gc_malloc(request_size);
    }

    int i;
    int size =  gc->gen0->size;
    for(i = 0; i < size; i++) {
        if(ptr == vector_get(gc->gen0, i)) {

            void *mem = realloc(ptr, request_size);
            vector_set(gc, i, mem);
            
            malCount++;
            if(malCount % markSweep == 0) {
                mark_and_sweep()
                malCount = 0;
            }

            return mem;
        }
    }

    return NULL;
}

void gc_free(void *ptr) {
    // no-op by design
}

