#include "debug.h"
#include "allocator.h"
#include "simulator.h"
#include "segfreelist.h"
#include "utils.h"
#include <stdlib.h>

// TODO: Decide whether seg_add should coalesce and do it here if it doesn't
void *addpage() {
    void *pghdr = ye_sbrk(PAGE_SIZE);
    if (pghdr == (void *) -1) {
        return NULL; // errno set to ENOMEM by ye_sbrk
    }
    prepare(pghdr, PAGE_SIZE, 0); 
    seg_add(pghdr);
    return pghdr;
}

void prepare(ye_header *hdr, size_t rsize, bool alloc) {
    ye_header *ftr = FOOTER(hdr);
    hdr->size = ftr->size = rsize >> 4;
    hdr->alloc = ftr->alloc = alloc;
}

// sets the new header to free and the old header to allocated by default.
ye_header *split(ye_header *hdr, size_t rsize) {
    ye_header *newhdr = ((void *) hdr) + size;
    prepare(newhdr, BLKSIZE(hdr) - rsize, 0);
    try_coalesce_next(newhdr);
    prepare(hdr, rsize, 1);
    return newhdr;
}

bool try_coalesce_next(ye_header *hdr) {
    ye_header *nexthdr = nextblock(blockhdr);
    if(nexthdr != NULL && !ALLOCATED(nexthdr)) {
        seg_rm(nexthdr);
        coalesce(blockhdr, nexthdr);
    }
    seg_add(blockhdr);    
}

// should this take a block that is or isn't in the segfreelist?
bool try_coalesce_prev(ye_header *hdr) {
    ye_header *prevhdr = prevblock(hdr);
    if(prevhdr != NULL && !ALLOCATED(prevhdr)) {
        seg_rm(nexthdr);
        coalesce(blockhdr, nexthdr);
    }
    seg_add(blockhdr);
}