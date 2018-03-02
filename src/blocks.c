#include "blocks.h"

#include <stdlib.h>
#include <stdbool.h>

#include "simulator.h"

// TODO: Find a better name for this function
void prepare(ye_header *hdr, size_t rsize, bool alloc) {
    ye_header *ftr = (void *) hdr + rsize;
    hdr->size = ftr->size = rsize >> 4;
    hdr->alloc = ftr->alloc = alloc;
}

ye_header *nextblock(ye_header *hdr) {
    ye_header *nexthdr = hdr + BLOCKSIZE(hdr);
    if (PAYLOAD(nexthdr) > heap_max()) {
        return NULL;  // in the wilderness block (or corruption occurred)
    }
    return nexthdr;
}

// TODO fix this.. header takes payload pointer and return header!!
ye_header *prevblock(ye_header *hdr) {
    size_t prevsize = BLOCKSIZE(hdr--); // look in the footer for block size.
    ye_header *prevhdr = (void *) hdr - prevsize; // avoid multiplying by sizeof ye_header
    if((void *) prevhdr < heap_min()) {
        return NULL;
    }
    return prevhdr;
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

// takes a rounded size. hdr is allocated.
void try_split_coalesce_forwards(ye_header *hdr, size_t rsize) {
    if ((BLOCKSIZE(hdr) - rsize) >= MIN_BLOCK_SIZE) {
        ye_header *newhdr = ((void *) hdr) + rsize;
        prepare(newhdr, BLOCKSIZE(hdr) - rsize, 0);
        prepare(hdr, rsize, 1);
        try_coalesce_forwards(newhdr);
    }
}
