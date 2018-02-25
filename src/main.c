#include <stdio.h>
#include <string.h>
#include "simulator.h"
#include "allocator.h"
#include <stdlib.h>

#include "seglist.h"

/*
 * TODO:
 *
 * - Replace the old criterion tests
 * - Rewrite debug functions to print out state of lists
 * - Clean up some spots with messy formatting / too much macro usage.
 * - Coalescing properly, in both directions
 *
 * Extra stuff, if I have time
 * - Redo the headers -- this is big!
 * - magic numbers to check for corruption
 * 
 * Already done
 * - Set up tests with criterion (includes Makefile work). DONE
 * - Add glibc-style linear/logarithmic bin sizes. DONE
 * - add calloc. DONE
 * - Set actual errno instead of ye_errno. DONE
 *
 */
int main(int argc, char const *argv[]) {
    setup();
    void *x = ye_malloc(sizeof(double) * 8);
    // ye_varprint(x);
    // ye_snapshot();
    void *z = ye_realloc(x, sizeof(int));
    // ye_varprint(y);
    // ye_snapshot();
    (void) z;
    teardown();
    return 2;
}
