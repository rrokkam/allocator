#include <errno.h>
#include <stdlib.h>
#include "simulator.h"
#include "debug.h"
#include "allocator.h"

int main(int argc, char *argv[]) {
    setup();
    errno = 0;

    /* Insert workload here (ex: test cases) */

    teardown();
}
