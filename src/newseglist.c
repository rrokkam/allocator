#include "debug.h"
#include "newseglist.h"
#include "allocator.h"

#define NUM_SMALL_LISTS 6 // lists with blocks of only one size
#define NUM_LISTS 11 // includes small lists

freelist seglist[NUM_LISTS];

/* 
 * Initialize segmented free list with mins and maxes
 * from 0 to 5: linearly spaced. 16, 24, 32, 40, 48, 56.
 * after 6: logarithmically spaced. 64, 128, 256, 512, ...
 *
 * TODO: Might be worth trying to do this with a macro.. this seems like it
 * could/should be done at compile-time.
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
    return seglist[seg_index(BLOCKSIZE(blockhdr))].head;
}

void seg_add(ye_header *blockhdr) {
    int index = seg_index(BLOCKSIZE(blockhdr));
    ye_header *old_head = seg_free_list[index].head;
    seg_free_list[index].head = blockhdr;
    blockhdr->next = old_head;
    blockhdr->prev = NULL;
    if (old_head != NULL) {
        old_head->prev = blockhdr;  
    }
}

void seg_rm(ye_header *blockhdr) {
    int index = seg_index(BLOCKSIZE(blockhdr));
    ye_header *prevhdr = blockhdr->prev;
    ye_header *nexthdr = blockhdr->next;
    if (prevhdr != NULL) {
        prevhdr->next = nexthdr;
    } else {
        seg_free_list[index].head = nexthdr;
    }
    if (nexthdr != NULL) {
        nexthdr->prev = prevhdr;
    }
}

// seg_finds
