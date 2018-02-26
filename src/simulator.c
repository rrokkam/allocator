#include <errno.h>
#include <stdlib.h>
#include "simulator.h"
#include "segfreelist.h"

/* The starting, current, and max brk pointer values */
void *min_brk;
void *cur_brk;
void *max_brk;

// will automatically mmap if size is too large.
void setup() {
    min_brk = malloc(NUM_PAGES * PAGE_SIZE);
    cur_brk = min_brk + PAGE_SIZE;
    max_brk = min_brk + NUM_PAGES * PAGE_SIZE;
    seg_init();
}

void teardown() {
    free(min_brk);
    min_brk = cur_brk = max_brk = NULL;
}

void *heap_min() {
    return min_brk;
}

void *heap_max() {
    return max_brk;
}

void *ye_sbrk(size_t increment) {
    void *new_brk = cur_brk + increment;
    if(new_brk <= max_brk) {
        cur_brk = new_brk;
        return cur_brk;
    } else {
        errno = ENOMEM;
        return (void *) -1;
    }
}
