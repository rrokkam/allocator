#include <errno.h>
#include <stdlib.h>
#include "simulator.h"
#include "segfreelist.h"

/* The starting, current, and max brk pointer values */
void *min_brk;
void *cur_brk;
void *max_brk;

freelist seglist[NUM_LISTS];

// Will automatically mmap if size is too large.
// Eight bytes padding at start is for double word alignment.
void setup() {
    min_brk = cur_brk = 8 + malloc(NUM_PAGES * PAGE_SIZE + 8);
    max_brk = min_brk + NUM_PAGES * PAGE_SIZE;
    seg_init();
    seg_add(add_page());
}

void teardown() {
    free(min_brk - 8);
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
        void *old_brk = cur_brk;
        cur_brk = new_brk;
        return old_brk;
    } else {
        errno = ENOMEM;
        return (void *) -1;
    }
}
