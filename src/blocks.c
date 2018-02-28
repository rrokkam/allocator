#include "blocks.h"

#include <stdlib.h>

#include "simulator.h"

ye_header *nextblock(ye_header *hdr) {
    void *nexthdr = hdr + BLOCKSIZE(hdr);
    if (nexthdr + YE_HEADER_SIZE > heap_max()) {
        return NULL;
    }
    return nexthdr;
}

ye_header *prevblock(ye_header *hdr) {
    void *prevftr = hdr - YE_HEADER_SIZE;
    if(prevftr < heap_min() ||
        BLOCKSIZE(prevftr) < MIN_BLOCK_SIZE) {
        return NULL;
    }
    void *prevhdr = HEADER(prevftr);
    if(prevhdr < heap_min()) {
        return NULL;
    }
    return prevhdr;
}

void try_coalesce_bidir(ye_header *hdr) {
    // filler to suppress errors
    if (prevblock(hdr) != NULL)
        hdr++;
}

void try_coalesce_forwards(ye_header *hdr, ye_header *nexthdr) {
    //
}

void try_coalesce_backwards(ye_header *hdr){
    //

} // TODO: assumes you're in the free list!!

void try_split_coalesce_forwards(ye_header *hdr, size_t size) {
    //
}
