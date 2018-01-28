#include <stdio.h>
#include "simulator.h"
#include "allocator.h"
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    setup();
    void *x = sf_malloc(sizeof(double) * 8);
    // sf_varprint(x);
    // sf_snapshot();
    void *y = sf_realloc(x, sizeof(int));
    // sf_varprint(y);
    // sf_snapshot();
    (void) y;
    teardown();
    return 2;
}
