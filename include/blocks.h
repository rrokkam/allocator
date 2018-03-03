#ifndef BLOCKS_H
#define BLOCKS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define SIZE_BITS 60 // realistically this can only go up to 16. set to 28 on 32 bit system
#define UNUSED 3
#define ALLOCATED_BIT 1

#define HEADER_SIZE 8  // In bytes. Doesn't include next/prev pointers!

#define MIN_PAYLOAD_SIZE 16  // Blocks need to be double word aligned.
#define MIN_BLOCK_SIZE (2 * HEADER_SIZE + MIN_PAYLOAD_SIZE)
#define MAX_BLOCK_SIZE ((uint16_t) -1)

#define HEADER(payload) ((void *) payload - HEADER_SIZE)
#define PAYLOAD(hdr) ((void *) hdr + HEADER_SIZE)
#define FOOTER(hdr) ((void *) hdr + BLOCKSIZE(hdr) - HEADER_SIZE)
#define ALLOCATED(hdr) (((ye_header *) hdr)->alloc)
#define BLOCKSIZE(hdr) (((ye_header *) hdr)->size << 4)

/* Round payload size up to the nearest multiple of 16 */
#define ROUND16(size) (((size_t) size & ~0x0F) + 16 * (((size_t) size & 0x0F) != 0))

/* 
 * Struct to represent a header or footer. If the allocated bit is set to 1,
 * then the next and prev fields are unused.
 */
typedef struct ye_header {
    uint64_t  alloc : ALLOCATED_BIT;
    uint64_t unused : UNUSED;
    uint64_t   size : SIZE_BITS;
    struct ye_header *next;
    struct ye_header *prev;
} __attribute__((packed)) ye_header;

/*
 * Calculates the size of the block needed to deliver a payload of size size,
 * including rounding for double word alignment and space for the header and
 * footer.
 */
size_t reqsize(size_t size);

/*
 * Set the size and allocated bit of a block. This function updates both the
 * header and footer to have size rsize and allocated bit equal to alloc.
 */
void set_block(ye_header *hdr, size_t rsize, bool alloc);

/*
 * Return the block in memory immediately after hdr (which might not be free).
 */
ye_header *nextblock(ye_header *hdr);

/*
* Return the block in memory immediately before hdr (which might not be free).
 */
ye_header *prevblock(ye_header *hdr);

/*
 * These methods check whether the next or previous blocks (or both, 
 * according to the name) are free, and attempt to coalesce the blocks 
 * together. They will add the result to the free list, regardless of
 * whether the coalescing was successful.
 * 
 * The hdr parameter must refer to an allocated block.
 *
 * For try_split_coalesce_forwards, the block is first split into two
 * pieces if possible, the first of which is of size rsize. The second
 * block is added to the free list, if it is made. The first is not.
 * 
 */
void try_coalesce_bidir(ye_header *hdr);

void try_coalesce_forwards(ye_header *hdr);

ye_header *try_coalesce_backwards(ye_header *hdr); // hdr is free but NOT in the free list.

void try_split_coalesce_forwards(ye_header *hdr, size_t rsize);

#endif /* BLOCKS_H */