#ifndef BLOCKS_H
#define BLOCKS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define SIZE_BITS 60 // realistically this can only go up to 16. set to 28 on 32 bit system
#define UNUSED 3
#define ALLOCATED_BIT 1

#define MIN_PAYLOAD_SIZE 16  // blocks need to be double word aligned.
#define MIN_BLOCK_SIZE (2 * sizeof(ye_header) + MIN_PAYLOAD_SIZE)
#define MAX_BLOCK_SIZE ((uint16_t) -1) 


// TODO: choose less confusing names
/* Get the header of a block from a payload pointer */
#define HEADER(payload) ((void *) payload - sizeof(ye_header))

/* Get the footer of a block from the header. */
#define FOOTER(hdr) ((void *)hdr + BLOCKSIZE(hdr) - sizeof(ye_header))

/* Evaluate to 1 if allocated, and 0 if free */
#define ALLOCATED(hdr) (((ye_header *) hdr)->alloc)

/* Get the size of a block from the header */
#define BLOCKSIZE(hdr) (((ye_header *)hdr)->size << 4)

typedef struct ye_header {
    uint64_t  alloc : ALLOCATED_BIT;
    uint64_t unused : UNUSED;
    uint64_t   size : SIZE_BITS;
    struct ye_header *next;
    struct ye_header *prev;
} __attribute__((packed)) ye_header;

void prepare(ye_header *hdr, size_t rsize, bool alloc);

ye_header *nextblock(ye_header *hdr);

ye_header *prevblock(ye_header *hdr);

void try_coalesce_bidir(ye_header *hdr);

void try_coalesce_forwards(ye_header *hdr);

ye_header *try_coalesce_backwards(ye_header *hdr); // hdr is free but NOT in the free list.

void try_split_coalesce_forwards(ye_header *hdr, size_t rsize);

#endif /* BLOCKS_H */