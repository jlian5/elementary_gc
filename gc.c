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
    vector *references;
} gc_metadata_t;

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
    // mark_and_sweep();
}

void *gc_calloc(size_t num_elements, size_t element_size) {

}

void *gc_realloc(void *ptr, size_t request_size) {

}

void gc_free(void *ptr) {
    // no-op by design
}

