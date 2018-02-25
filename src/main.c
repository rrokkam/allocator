#include <stdio.h>
#include "simulator.h"
#include "allocator.h"
#include <stdlib.h>

#include "newseglist.h"

int rnd(int num) {
    return (num & 0x07) | 0x08;
}

int main(int argc, char const *argv[]) {
    setup();
    printf("%d", (int) sizeof(int *));
/*    void *x = ye_malloc(sizeof(double) * 8);
    // ye_varprint(x);
    // ye_snapshot();
    void *y = ye_realloc(x, sizeof(int));
    // ye_varprint(y);
    // ye_snapshot();
    (void) y;
    teardown();*/
    return 2;
}
