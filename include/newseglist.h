#ifndef NEWSEGLIST_H
#define NEWSEGLIST_H

#include "helpers.h"
#include "allocator.h"

#define NUM_SMALL_LISTS 7 // lists with blocks of only one size
#define NUM_LISTS 11 // includes small lists

/* Struct to store a free list */
typedef struct {
    ye_header *head;
    uint16_t min;
    uint16_t max;
} freelist;

/* */
extern freelist seglist[NUM_LISTS];

void seg_init();
void nseg_insert(ye_header *blockhdr);
void nseg_remove(ye_header *blockhdr);
ye_header *nseg_findspace(size_t size);

#endif /* NEWSEGLIST_H */