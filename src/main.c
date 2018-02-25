#include <stdio.h>
#include "simulator.h"
#include "allocator.h"
#include <stdlib.h>

#include "newseglist.h"

/*
 * TODO:
 *
 * - Set up tests with criterion (includes Makefile work)
 * - Replace the old criterion tests
 * - Rewrite debug functions to print out state of lists
 * - Clean up some spots with messy formatting / too much macro usage.
 * - Coalescing properly, in both directions
 *
 * Extra stuff, if I have time
 * - Add glibc-style linear/logarithmic bin sizes. DONE
 * - Redo the headers
 * - add calloc
 * - magic numbers to check for corruption
 * - Set actual errno instead of ye_errno
 * 
 */
int main(int argc, char const *argv[]) {
    setup();
    void *x = ye_malloc(sizeof(double) * 8);
    // ye_varprint(x);
    // ye_snapshot();
    void *y = ye_realloc(x, sizeof(int));
    // ye_varprint(y);
    // ye_snapshot();
    (void) y;
    teardown();
    return 2;
}
