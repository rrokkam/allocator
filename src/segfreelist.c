#include "segfreelist.h"

#include <errno.h>

#include "simulator.h"
#include "debug.h"
#include "allocator.h"
#include "utils.h"

freelist seglist[NUM_LISTS];

/* 
 * Initialize segmented free list with mins and maxes
 * from 0 to 5: linearly spaced. 16, 24, 32, 40, 48, 56.
 * after 6: logarithmically spaced. 64, 128, 256, 512, ...
 *
 * TODO: Might be worth trying to do this with a macro.. this seems like it
 * could/should be done at compile time.
 */
void seg_init() {
    for (int i = 0; i < NUM_SMALL_LISTS; i++) {
        seglist[i] = (freelist) {NULL, (i << 3) + 16, (i << 3) + 16};
    }
    for (int i = NUM_SMALL_LISTS; i < NUM_LISTS; i++) {
        seglist[i] = (freelist) {NULL, 1 << i, (1 << (i + 1)) - 8};
    }
    seglist[NUM_LISTS - 1].max = -1;
}

/* Don't give this a size lower than the minimum!! */
int seg_index(size_t rsize) {
    if (rsize < seglist[NUM_SMALL_LISTS].min) {
        return (rsize >> 3) - 2;
    } else if (rsize > seglist[NUM_LISTS - 1].min) {
        return NUM_LISTS - 1;
    } else { // compute first bit set. This is fls(size) on BSD.
        int index;
        for (index = 0; rsize != 1; index++) {
            rsize >>= 1;
        }
        return index;
    }
}

ye_header *seg_head(ye_header *hdr) {
    return seglist[seg_index(BLOCKSIZE(hdr))].head;
}

void seg_add(ye_header *hdr) {
    int index = seg_index(BLOCKSIZE(hdr));
    ye_header *old_head = seglist[index].head;
    seglist[index].head = hdr;
    hdr->next = old_head;
    hdr->prev = NULL;
    if (old_head != NULL) {
        old_head->prev = hdr;  
    }
}

void seg_rm(ye_header *hdr) {
    int index = seg_index(BLOCKSIZE(hdr));
    ye_header *prevhdr = hdr->prev;
    ye_header *nexthdr = hdr->next;
    if (prevhdr != NULL) {
        prevhdr->next = nexthdr;
    } else {
        seglist[index].head = nexthdr;
    }
    if (nexthdr != NULL) {
        nexthdr->prev = prevhdr;
    }
}

static void *add_page() {
    ye_header *hdr = ye_sbrk(PAGE_SIZE);
    if (hdr == (void *) -1) {
        return NULL; // errno set to ENOMEM by ye_sbrk
    }
    ye_header *ftr = FOOTER(hdr);
    hdr->size = ftr->size = PAGE_SIZE >> 4;
    hdr->alloc = ftr->alloc = 0;
    try_coalesce_backwards(hdr); // TODO: assumes you're in the free list!!
    seg_add(hdr);
    return hdr;
}

// TODO: seg_find with calling ye_sbrk properly.
ye_header *seg_find(size_t size) {
    ye_header *hdr, *pghdr, *newhdr;
    for (int i = seg_index(size); i < NUM_LISTS; i++) {
        hdr = seglist[i].head;
        for (hdr = seglist[i].head; hdr != NULL; hdr = hdr->next) {
            if (BLOCKSIZE(hdr) >= size) {
                return hdr;
            }
        }
    }

    do { // Didn't find a block of sufficient size, time to ye_sbrk a page at a time.
        if ((pghdr = add_page()) == NULL) {
            errno = ENOMEM;
            return NULL;
        }
        hdr = prevblock(pghdr); // try_coalesce_down
        if (hdr != NULL && !ALLOCATED(hdr)) { // handles before getheapstart
            seg_rm(hdr); // extra inserts and removes are a little silly
                                    // but are needed for coalesce to work properly.
            try_coalesce_forwards(hdr, pghdr);
            seg_add(hdr);
            newhdr = hdr;
        } else {
            seg_add(pghdr);
            newhdr = pghdr;
        }
    } while (BLOCKSIZE(newhdr) < size); // don't need to check the rest, this is biggest
    seg_rm(newhdr);
    return newhdr;
}
