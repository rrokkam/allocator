#include "debug.h"
#include "allocator.h"
#include "simulator.h"
#include "segfreelist.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


void ye_snapshot() {
    ye_header *head;
    for (int index = 0; index < NUM_LISTS; index++) {
        head = seglist[index].head;
        if (head != NULL) {
            info("FREE LIST %d:", index);
            for (ye_header *block = head; block != NULL; block = block->next) {
                ye_blockprint(block);
            }
        }
    }
}

void ye_blockprint(ye_header *block) {
    info("Block at %p: \nsize: %u \nallocated: %s \nnext: \n%p prev: %p",
         block, block->block_size, block->allocated, block->next, block->prev);
}

void ye_varprint(void *data) {
    ye_blockprint(data - YE_HEADER_SIZE);
}


/*
 * Checks if the block is a valid block.
 */
bool valid_block(void *ptr) {
    if(ptr == NULL) return false;

    if(ptr < heap_min()) return false;
    if(ptr + MIN_BLOCK_SIZE > heap_max()) return false; // duplicate check for ftr

    ye_header *blockhdr = (ye_header *) ptr;
    ye_header *blockftr = FOOTER(blockhdr);
    size_t hdrblocksize = BLOCKSIZE(blockhdr);
    size_t ftrblocksize = BLOCKSIZE(blockftr);

    if((void *) blockftr - MIN_BLOCK_SIZE + YE_HEADER_SIZE < heap_min()) return false;
    if((void *) blockftr + YE_HEADER_SIZE > heap_max()) return false;

    if(hdrblocksize != ftrblocksize) return false;
    if(blockhdr->allocated != blockftr->allocated) return false;

    size_t sz = PAYLOADSIZE(hdrblocksize);
    if(BLOCKSIZE(blockhdr) < MIN_BLOCK_SIZE) return false;
    if(!VALIDSIZE(hdrblocksize)) return false;

    if(!blockhdr->allocated) return true; // requested size bits are don't caress
    if(blockftr->requested_size > sz) return false;
    return true;
}

/*
 * Initialize a block header and footer with the provided bit fields.
 */
void prepare(void *blockhdr, size_t requested_size,
    size_t block_size, bool allocated) {
    ye_header *hdr = (ye_header *)blockhdr;
    hdr->block_size = block_size >> 4;
    hdr->allocated = allocated;

    ye_header *ftr = FOOTER(hdr);
    ftr->requested_size = requested_size;
    ftr->block_size = hdr->block_size;
    ftr->allocated = hdr->allocated;
}

/**
 * Assumes the provided block and the next block are valid and free,
 * and neither are in free lists. nexthdr should be nextblock(blockhdr).
 */
void coalesce(void *firsthdr, void *secondhdr) {
    prepare(firsthdr, 0, BLOCKSIZE(firsthdr) + BLOCKSIZE(secondhdr), 0);
}

void try_coalesce_up(void *blockhdr) {
    ye_header *nexthdr = nextblock(blockhdr);
    if(nexthdr != NULL && !ALLOCATED(nexthdr)) {
        seg_rm(nexthdr);
        coalesce(blockhdr, nexthdr);
    }
    seg_add(blockhdr);
}

/**
 * Return the minimum block size to get the requested payload size.
 * Assumes the provided number size is a valid size; no error checking.
 */
size_t required_size(size_t size) {
    if(size % 16 == 0) return size + YE_OVERHEAD;
    return (((size >> 4) + 1) << 4) + YE_OVERHEAD; // round up
}

/**
 * Return true if the block can be split, with the first block of length size,
 * without creating a splinter, and false otherwise. Assumes the size is a valid
 * size for a block and does no error checking.
 */
bool can_split(void *blockhdr, size_t reqsize) {
    return (BLOCKSIZE(blockhdr) - reqsize) >= MIN_BLOCK_SIZE;
}

/**
 * Return a pointer to the new free block. NO ERROR CHECKING FOR SIZE.
 * Does NOT prepare the old block header.
 */
void *split(void *blockhdr, size_t size) {
    ye_header *newhdr = blockhdr + size;
    prepare(newhdr, 0, BLOCKSIZE(blockhdr) - size, 0);
    prepare(blockhdr, 0, size, 0);
    return newhdr;
}

void *addpage() {
    void *pghdr = ye_sbrk();
    if(pghdr == (void *) -1) return NULL;
    prepare(pghdr, 0, /*PAGE_SZ*/ 999, 0); // TODO: move this into seglist.
    return pghdr;
}

void *nextblock(void *blockhdr) {
    void *nexthdr = blockhdr + BLOCKSIZE(blockhdr);
    if(nexthdr + YE_HEADER_SIZE > heap_max()
        || !valid_block(nexthdr)) return NULL;
    return nexthdr;
}

void *prevblock(void *blockhdr) {
    void *prevftr = blockhdr - YE_HEADER_SIZE;
    if(prevftr < heap_min() ||
        BLOCKSIZE(prevftr) < MIN_BLOCK_SIZE) return NULL;
    void *prevhdr = HEADER(prevftr);
    if(prevhdr < heap_min() || !valid_block(prevhdr)) return NULL;
    return prevhdr;
}
