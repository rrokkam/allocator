#include "debug.h"
#include "allocator.h"
#include "simulator.h"
#include "segfreelist.h"
#include "utils.h"
#include <stdlib.h>

void prepare(ye_header *hdr, size_t rsize, bool alloc) {
    ye_header *ftr = FOOTER(hdr);
    hdr->size = ftr->size = rsize >> 4;
    hdr->alloc = ftr->alloc = alloc;
}

void *nextblock(void *hdr) {
    void *nexthdr = hdr + BLKSIZE(hdr);
    if (nexthdr + YE_HEADER_SIZE > heap_max()) {
        return NULL;
    }
    return nexthdr;
}

void *prevblock(void *hdr) {
    void *prevftr = hdr - YE_HEADER_SIZE;
    if(prevftr < heap_min() ||
        BLKSIZE(prevftr) < MIN_BLOCK_SIZE) {
        return NULL;
    }
    void *prevhdr = HEADER(prevftr);
    if(prevhdr < heap_min()) {
        return NULL;
    }
    return prevhdr;
}

// sets coalesce result to free
void coalesce(ye_header *hdr1, ye_header *hdr2) {
    prepare(hdr1, BLKSIZE(hdr1) + BLKSIZE(hdr1), hdr1->alloc);
}

void try_coalesce_next(ye_header *hdr) {
    ye_header *nexthdr = nextblock(hdr);
    if(nexthdr != NULL && !ALLOCATED(nexthdr)) {
        seg_rm(nexthdr);
        coalesce(hdr, nexthdr);
    }
    seg_add(hdr);    
}

// should this take a block that is or isn't in the segfreelist?
void try_coalesce_prev(ye_header *hdr) {
    ye_header *prevhdr = prevblock(hdr);
    if(prevhdr != NULL && !ALLOCATED(prevhdr)) {
        seg_rm(hdr);
        coalesce(prevhdr, hdr);
    }
    seg_add(hdr);
}

void try_coalesce(ye_header *hdr) {
    try_coalesce_prev(hdr);
    try_coalesce_prev(hdr);
}

// sets the new header to free and the old header to allocated by default.
ye_header *split(ye_header *hdr, size_t rsize) {
    ye_header *newhdr = ((void *) hdr) + rsize;
    prepare(newhdr, BLKSIZE(hdr) - rsize, 0);
    try_coalesce_next(newhdr);
    prepare(hdr, rsize, 1);
    return newhdr;
}

// TODO: Decide whether seg_add should coalesce and do it here if it doesn't
void *add_page() {
    void *pghdr = ye_sbrk(PAGE_SIZE);
    if (pghdr == (void *) -1) {
        return NULL; // errno set to ENOMEM by ye_sbrk
    }
    prepare(pghdr, PAGE_SIZE, 0); 
    try_coalesce_prev(pghdr); // TODO: assumes you're in the free list!!
    seg_add(pghdr);
    return pghdr;
}
