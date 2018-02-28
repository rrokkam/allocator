#include "blocks.h"

#include <stdlib.h>

#include "simulator.h"


ye_header *nextblock(ye_header *hdr) {
    ye_header *nexthdr = hdr + BLOCKSIZE(hdr);
    if ((void *) ++nexthdr > heap_max()) {
        return NULL;  // in the wilderness block (or corruption occurred)
    }
    return nexthdr;
}

// TODO fix this.. header takes payload pointer and return header!!
ye_header *prevblock(ye_header *hdr) {
    ye_header *prevftr = hdr--;
    ye_header *prevhdr = HEADER(hdr--);  // casting shouldn't matter at runtime
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


// doing forwards will add you to the free list, which we would
// need to immediately remove.
void try_coalesce_bidir(ye_header *hdr) {
    ye_header *nexthdr = nextblock(hdr);
    if(nexthdr != NULL && !ALLOCATED(nexthdr)) {
        seg_rm(nexthdr);  // after this, neither are in the free list.
        coalesce(hdr, nexthdr);
    }
    try_coalesce_backwards(hdr);
}

// hdr cannot be in the free list.
// should this take nexthdr?
void try_coalesce_forwards(ye_header *hdr, ye_header *nexthdr) {
    if(nexthdr != NULL && !ALLOCATED(nexthdr)) {
        seg_rm(nexthdr);  // after this, neither are in the free list.
        coalesce(hdr, nexthdr);
    }
    seg_add(hdr);
}

// hdr cannot be in the free list
void try_coalesce_backwards(ye_header *hdr) {
    ye_header *prevhdr = prevblock(hdr);
    if (prevhdr != NULL && !ALLOCATED(prevhdr)) {
        seg_rm(prevhdr);
        coalesce(prevhdr, hdr);
        seg_add(prevhdr); 
    } else {
        seg_add(hdr);
    }
}

// takes a rounded size.
void try_split_coalesce_forwards(ye_header *hdr, size_t size) {
    if ((BLOCKSIZE(hdr) - (size_t) size) >= MIN_BLOCK_SIZE) {
        // TODO: fill this in
        try_coalesce_forwards(newhdr);
    }
}
