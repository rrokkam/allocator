#include <errno.h>
#include <stdlib.h>
#include "simulator.h"

void *start;
void *end;
void *max;

/*
 * Any program using the sfmm library must call this function ONCE
 * before issuing any allocation requests. This function DOES NOT
 * allocate any space to your allocator.
 */
void setup() {
    start = malloc(NUM_PAGES * PAGE_SIZE);
    end = start + PAGE_SIZE;
    max = start + NUM_PAGES * PAGE_SIZE;
}

/*
 * Any program using the sfmm library must call this function ONCE
 * after all allocation requests are complete. If implemented cleanly,
 * your program should have no memory leaks in valgrind after this function
 * is called.
 */
void teardown() {
    free(start);
    start = end = max = NULL;
}

/*
 * @return The starting address of the heap for your allocator.
 */
void *get_heap_start() {
    return start;
}

/*
 * @return The ending address of the heap for your allocator.
 */
void *get_heap_end() {
    return end;
}

/*
 * This function changes the position of your program's break.
 * Calling this function increments the break by 1 page and updates
 * the heap start and end variables, which can be accessed through
 * get_heap_start() and get_heap_end().
 *
 * @return On success, this function returns the previous program break.
 * If the break was increased, this value is a pointer to the start of the newly
 * allocated memory. On error, (void *) -1 is returned and sf_errno is set to ENOMEM.
 */
void *my_sbrk() {
    if(end + PAGE_SIZE <= max) {
        void *ret = end;
        end += PAGE_SIZE;
        return ret;
    } else {
        errno = ENOMEM;
        return (void *) -1;
    }
}

