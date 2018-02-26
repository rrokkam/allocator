#ifndef SEGLIST_H
#define SEGLIST_H

#include <stdlib.h>
#include <stdint.h> // TODO: figure out number types

#define SIZE_BITS 60
#define UNUSED 3
#define ALLOCATED_BIT 1

#define YE_HEADER_SIZE (2 * (SIZE_BITS + UNUSED + ALLOCATED_BIT) >> 3)  // 8 bytes

#define NUM_SMALL_LISTS 6 // lists with blocks of only one size
#define NUM_LISTS 11 // includes small lists

typedef struct ye_header {
    uint64_t  alloc : ALLOCATED_BIT;
    uint64_t unused : UNUSED;
    uint64_t   size : SIZE_BITS;
    struct ye_header *next;
    struct ye_header *prev;
} __attribute__((packed)) ye_header;

/* Struct to store a free list */
typedef struct {
    ye_header *head;
    uint16_t min;
    uint16_t max;
} freelist;

extern freelist seglist[NUM_LISTS];

void seg_init();
void seg_add(ye_header *blockhdr);
void seg_rm(ye_header *blockhdr);
ye_header *seg_find(size_t size);

#endif /* SEGLIST_H */
