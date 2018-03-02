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



    teardown();
}
