#ifndef SEGLIST_H
#define SEGLIST_H

#include <stdlib.h>
#include <stdint.h>

#include "blocks.h"

/* Struct to store a free list */
typedef struct {
    ye_header *head;
    uint16_t min;
    uint16_t max;
} freelist;

/* 
 * Initialize segmented free list with mins and maxes from 0 to 5: linearly 
 * spaced. 16, 24, 32, 40, 48, 56. After 6: logarithmically spaced. 64, 128, 
 * 256, 512, ...
 * 
 * Call this once (before using seg_add or seg_rm).
 */
void seg_init();

/* 
 * Given a block size, return the index of the segmented free list that the 
 * block would be in.
 *
 * The behavior may be unexpected if rsize is less than MIN_BLOCK_SIZE.  
 */
int seg_index(size_t rsize);

/*
 * Add the given header to the correct list in the segmented free list, and 
 * mark it as free.
 */
void seg_add(ye_header *hdr);

/*
 * Remove the given header from the segmented free list, and mark it as
 * allocated.
 */
void seg_rm(ye_header *hdr);

/*
 * Find a free block larger than rsize, split any extra memory off of it if
 * possible, and remove the header from the free lists.
 */
ye_header *seg_find(size_t rsize);

/*
 * Add a page's worth of memory to the end of the heap, and merge the new
 * memory into the free list.
 */
void *add_page();

#endif /* SEGLIST_H */
