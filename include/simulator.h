#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "segfreelist.h"

#define NUM_PAGES 16  // Set >= 1
#define PAGE_SIZE 1024  // Double word aligned and preferably a power of 2.

#define NUM_SMALL_LISTS 6  // lists with blocks of only one size (best-fit)
#define NUM_LISTS 11  // includes small lists

extern freelist seglist[NUM_LISTS];

/*
 * This function must be called once before any allocation requests are made.
 */
void setup();

/*
 * This function must be called once after any allocation requests are made.
 */
void teardown();

/*
 * Return the starting address of the heap.
 */
void *heap_min();

/*
 * Return the ending address of the heap.
 */
void *heap_max();

/*
 * This function changes the position of your program's break.
 * Calling this function increments the break by 1 page and updates
 * the brk pointer.
 *
 * On success, this function returns the previous program break.
 * If the break was increased, this value is a pointer to the start of the newly
 * allocated memory. On error, (void *) -1 is returned and errno is set to ENOMEM.
 */
void *ye_sbrk();

#endif