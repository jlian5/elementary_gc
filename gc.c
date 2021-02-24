#include "gc.h"
#include "vector.h"

/**
 * Metadata for all garbage collection objects
 */
typedef struct _gc_metadata {
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

    /**
     * These are vectors for each generation
     * Each vector has some data associated to describe the vector
     */
    
    struct generation *gen0;
    struct generation *gen1;
    struct generation *gen2;

    /**
     * Sets the data capacity for each vector
     */
    int capacity = 2048;
    gen0->max_data = capacity;
    gen1->max_data = capacity/2;
    gen2->max_data = capacity/4;

} gc_metadata_t;


typedef struct generation {

    //max amount of data for a vector until it needs to be mark and sweep
    int max_data;

    //current amount of data in the vector
    int current_size;

    //vector containing the data
    vector *data;
}
/**
 * Contains a list of the top level objects that are accessible.
 * TODO: change this to add different vectors for different aged objects
 */
static vector *top_level_objects;
static vector *grey_slots;
static vector *black_slots;

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

void *gc_malloc(size_t request_size) {
    //check if the request_size + gen0->current_size >  gen0->max_size
    //if true, call mark_and_sweep(); on gen0 if the amount of data allocated to gen0 exceeds its capacity and move the remains to gen1
    //then, call mark_and_sweep(); on gen1 etc... if the data exceeds these vectors max_size
}

void *gc_calloc(size_t num_elements, size_t element_size) {

}

void *gc_realloc(void *ptr, size_t request_size) {

}

void gc_free(void *ptr) {
    // no-op by design
}

