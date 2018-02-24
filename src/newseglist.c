#include "debug.h"
#include "newseglist.h"
#include "allocator.h"

freelist seglist[NUM_LISTS];

/* 
 * Initialize segmented free list with mins and maxes
 * from 0 to 6: linearly spaced. 16, 24, 32, 40, 48, 56, 64.
 * after 7: logarithmically spaced. 128, 256, 512, up to PAGE_SIZE.
 *
 * Might be worth trying to do this with a macro
 */ 
void seg_init() {
    for (int i = 0; i < NUM_SMALL_LISTS; i++) {
        seglist[i] = (freelist) {NULL, (i << 3) + 16, (i << 3) + 16};
    }
    for (int i = NUM_SMALL_LISTS; i < NUM_LISTS; i++) {
        seglist[i] = (freelist) {NULL, (1 << (i-1)) + 8, 1 << i};
    }
}
