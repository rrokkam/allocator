#include <errno.h>
#include <stdlib.h>
#include "simulator.h"
#include "debug.h"
#include "allocator.h"

/*
 * TODO:
 *
 * - Update the old criterion tests
 * - Document the new stuff
 * 
 * Already done
 * - Set up tests with criterion (includes Makefile work)
 * - Add glibc-style linear/logarithmic bin sizes
 * - add calloc
 * - Set actual errno instead of ye_errno
 * - Rewrite debug functions to print out state of lists
 * - Clean up some spots with messy formatting / too much macro usage.
 * - Coalescing properly, in both directions
 * - Redo the headers -- this is big!
 *
 */
int main(int argc, char *argv[]) {
    setup();
    errno = 0;

    /* Insert workload here */

    void *x = ye_malloc(sizeof(int) * 16);
    void *y = ye_realloc(x, sizeof(char));

//    cr_assert_not_null(y, "y is NULL!");
//    cr_assert(x == y, "Payload addresses are different!");

    ye_header *header = (ye_header*)((char*) y - 8);
//    cr_assert(header->alloc == 1, "Allocated bit is not set!");
//    cr_assert(header->size << 4 == 32, "Block size not what was expected!");

    freelist *fl = &seglist[seg_index(PAGE_SIZE - 48)];

    // There should be only one free block of size 4048 in list 9
//    cr_assert_not_null(fl->head, "No block in expected free list!");
//    cr_assert(fl->head->alloc == 0, "Allocated bit is set!");
//    cr_assert(fl->head->size << 4 == 4048, "Free block size not what was expected!");

    (void) header;
    (void) fl;

    teardown();
}
