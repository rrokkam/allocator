#include <stdlib.h>

/*
 * TODO:
 *
 * - New helper functionality:
 *     - on malloc: seg_find needs to split if possible, coalesce forwards (and mark orig block as allocated)
 *     - on free: set allocated to 0 and try to coalesce on both sides
 *     - on realloc lazy upsize: try to coalesce forwards
 *     - on realloc downsize: try to split and coalesce forwards
 *
 * - Functions that support this:
 *     - try_coalesce_forwards: if next block is free, coalesce. Either way, add it to the free list.
 *     - try_coalesce_backwards: if prev block is free, coalesce. Either way, add it to the free list. // in addpage
 *     - try_split_coalesce_forwards: if splittable, split and call try_coalesce_forwards on the new block.
 *     - try_coalesce_bidir: attempt to coalesce in both directions before adding to the free list.
 *
 * - Update the old criterion tests
 * - Document the new stuff
 * - extra: magic numbers to check for corruption
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
    exit(EXIT_SUCCESS); // placeholder for compilation
    // setup();
    // void *x = ye_malloc(sizeof(double) * 8);
    // // ye_varprint(x);
    // // ye_snapshot();
    // void *z = ye_realloc(x, sizeof(int));
    // // ye_varprint(y);
    // // ye_snapshot();
    // (void) z;
    // teardown();
    // return 2; // for testing..
}
