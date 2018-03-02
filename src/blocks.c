#include "blocks.h"

#include <stdlib.h>
#include <stdbool.h>

#include "simulator.h"

size_t reqsize(size_t size) {
    size_t blocksize = size + 2 * HEADER_SIZE;
    if (blocksize < MIN_BLOCK_SIZE) {
        return MIN_BLOCK_SIZE;
    } else {
        return ROUND(blocksize);
    }
}

// TODO: Find a better name for this function
void prepare(ye_header *hdr, size_t rsize, bool alloc) {
    void *hdrptr = hdr;
    ye_header *ftr = hdrptr + rsize - 8;
    hdr->size = ftr->size = rsize >> 4;
    hdr->alloc = ftr->alloc = alloc;
}

ye_header *nextblock(ye_header *hdr) {
    void *hdrptr = hdr;
    ye_header *nexthdr = hdrptr + BLOCKSIZE(hdr);
    if (PAYLOAD(nexthdr) > ye_sbrk(0)) {
        return NULL;  // in the wilderness block (or corruption occurred)
    }
    return nexthdr;
}

ye_header *prevblock(ye_header *hdr) {
    void *hdrptr = hdr;
    void *prevftrptr = hdrptr - HEADER_SIZE;
    size_t prevsize = BLOCKSIZE(prevftrptr); // look in the footer for block size.
    void *prevhdrptr = hdrptr - prevsize; // avoid multiplying by sizeof ye_header
    if(prevhdrptr < heap_min()) {
        return NULL;
    }
    return prevhdrptr;
}

// Assumes both blocks are adjacent and not in the free list.
static void coalesce(ye_header *hdr, ye_header *nexthdr) {
    ye_header *newftr = FOOTER(nexthdr);
    newftr->alloc = hdr->alloc;  // keep alloc state of first block.
    hdr->size += nexthdr->size;
    newftr->size = hdr->size;
}


// void coalesce(ye_header *hdr1, ye_header *hdr2) {
//     prepare(hdr1, BLOCKSIZE(hdr1) + BLOCKSIZE(hdr1), hdr1->alloc);
// }

// doing forwards will add you to the free list, which we would
// need to immediately remove.
void try_coalesce_bidir(ye_header *hdr) {
    ye_header *nexthdr = nextblock(hdr);
    if(nexthdr != NULL && !ALLOCATED(nexthdr)) {
        seg_rm(nexthdr);  // after this, neither are in the free list.
        coalesce(hdr, nexthdr);
    }
    try_coalesce_backwards(hdr);  // also adds to the free list.
}

// hdr cannot be in the free list.
// should this take nexthdr?
void try_coalesce_forwards(ye_header *hdr) {
    ye_header *nexthdr = nextblock(hdr);
    if(nexthdr != NULL && !ALLOCATED(nexthdr)) {
        seg_rm(nexthdr);  // after this, neither are in the free list.
        coalesce(hdr, nexthdr);
    }
    seg_add(hdr);
}

// hdr cannot be in the free list
// use this for the wilderness block
ye_header *try_coalesce_backwards(ye_header *hdr) {
    ye_header *prevhdr = prevblock(hdr);
    if (prevhdr != NULL && !ALLOCATED(prevhdr)) {
        seg_rm(prevhdr);
        coalesce(prevhdr, hdr);
        seg_add(prevhdr);
        return prevhdr;
    } else {
        seg_add(hdr);
        return hdr;
    }
}

// reduce the size.
// takes a rounded size. hdr is allocated.
void try_split_coalesce_forwards(ye_header *hdr, size_t rsize) {
    if ((BLOCKSIZE(hdr) - rsize) >= MIN_BLOCK_SIZE) {
        void *hdrptr = hdr;  // can't cast on the same line.
        ye_header *newhdr = hdrptr + rsize;
        prepare(newhdr, BLOCKSIZE(hdr) - rsize, 0);
        prepare(hdr, rsize, 1);
        try_coalesce_forwards(newhdr);
    }
}
