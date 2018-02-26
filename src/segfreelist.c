#include <errno.h>
#include "segfreelist.h"
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
int seg_index(size_t size) {
    if (size < seglist[NUM_SMALL_LISTS].min) {
        if (size & 0x07) { // divisible by 8
            return (size >> 3) - 1;
        } else {
            return (size >> 3) - 2;
        }
    } else if (size > seglist[NUM_LISTS - 1].min) {
        return NUM_LISTS - 1;
    } else { // compute first bit set. This is fls(size) on BSD.
        int index;
        for (index = 0; size != 1; index++) {
            size >>= 1;
        }
        return index;
    }
}

ye_header *seg_head(ye_header *blockhdr) {
    return seglist[seg_index(BLKSIZE(blockhdr))].head;
}

void seg_add(ye_header *blockhdr) {
    int index = seg_index(BLKSIZE(blockhdr));
    ye_header *old_head = seglist[index].head;
    seglist[index].head = blockhdr;
    blockhdr->next = old_head;
    blockhdr->prev = NULL;
    if (old_head != NULL) {
        old_head->prev = blockhdr;  
    }
}

void seg_rm(ye_header *blockhdr) {
    int index = seg_index(BLKSIZE(blockhdr));
    ye_header *prevhdr = blockhdr->prev;
    ye_header *nexthdr = blockhdr->next;
    if (prevhdr != NULL) {
        prevhdr->next = nexthdr;
    } else {
        seglist[index].head = nexthdr;
    }
    if (nexthdr != NULL) {
        nexthdr->prev = prevhdr;
    }
}

// TODO: seg_find with calling ye_sbrk properly.
ye_header *seg_find(size_t size) {
    ye_header *blockhdr, *pghdr, *newhdr;
    for (int i = seg_index(size); i < NUM_LISTS; i++) {
        blockhdr = seglist[i].head;
        for (blockhdr = seglist[i].head; blockhdr != NULL; blockhdr = blockhdr->next) {
            if (BLKSIZE(blockhdr) >= size) {
                return blockhdr;
            }
        }
    }

    do { // Didn't find a block of sufficient size, time to ye_sbrk a page at a time.
        if ((pghdr = addpage()) == NULL) {
            errno = ENOMEM;
            return NULL;
        }
        blockhdr = prevblock(pghdr); // try_coalesce_down
        if (blockhdr != NULL && !ALLOCATED(blockhdr)) { // handles before getheapstart
            seg_rm(blockhdr); // extra inserts and removes are a little silly
                                    // but are needed for coalesce to work properly.
            coalesce(blockhdr, pghdr);
            seg_add(blockhdr);
            newhdr = blockhdr;
        } else {
            seg_add(pghdr);
            newhdr = pghdr;
        }
    } while (BLKSIZE(newhdr) < size); // don't need to check the rest, this is biggest
    seg_rm(newhdr);
    return newhdr;
}
