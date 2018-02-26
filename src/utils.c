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

/* TODO: this is on one line for the compiler's sake.. how to fix? */
void ye_blockprint(ye_header *block) {
    info("Block at %p: \n\
         size: %u \n\
         allocated: %s \n\
         next: %p \n\
         prev: %p",
         block, block->size, block->alloc, block->next, block->prev);
}

void ye_varprint(void *data) {
    ye_blockprint(data - YE_HEADER_SIZE);
}

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

void prepare(ye_header *hdr, size_t size, bool alloc) {
    ye_header *ftr = FOOTER(hdr);
    hdr->size = ftr->size = size >> 4;
    hdr->alloc = ftr->alloc = alloc;
}
