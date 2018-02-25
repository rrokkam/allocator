#include <stdio.h>
#include <string.h>
#include "simulator.h"
#include "allocator.h"
#include <stdlib.h>

#include "segfreelist.h"

/*
 * TODO:
 *
 * - Replace the old criterion tests
 * - Clean up some spots with messy formatting / too much macro usage.
 * - Coalescing properly, in both directions
 * - Document the new stuff
 * - Redo the headers -- this is big!
 * - extra: magic numbers to check for corruption
 * 
 * Already done
 * - Set up tests with criterion (includes Makefile work)
 * - Add glibc-style linear/logarithmic bin sizes
 * - add calloc
 * - Set actual errno instead of ye_errno
 * - Rewrite debug functions to print out state of lists
 *
 */
int main(int argc, char const *argv[]) {
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
    // return 2;
}
