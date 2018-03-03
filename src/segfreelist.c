#include "segfreelist.h"

#include <errno.h>

#include "simulator.h"
#include "debug.h"
#include "allocator.h"

void seg_init() {
    for (int i = 0; i < NUM_SMALL_LISTS; i++) {
        seglist[i] = (freelist) {NULL, (i << 3) + 16, (i << 3) + 16};
    }
    for (int i = NUM_SMALL_LISTS; i < NUM_LISTS; i++) {
        seglist[i] = (freelist) {NULL, 1 << i, (1 << (i + 1)) - 8};
    }
    seglist[NUM_LISTS - 1].max = -1;
}

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

/*
 * Set the allocated bit in the given header.
 */
static void set_alloc(ye_header *hdr, bool val) {
    ye_header *ftr = FOOTER(hdr);
    hdr->alloc = ftr->alloc = val;
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
    set_alloc(hdr, 0);
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
    set_alloc(hdr, 1);
}

void *add_page() {
    ye_header *hdr = ye_sbrk(PAGE_SIZE);
    if (hdr == (void *) -1) {
        return NULL; // errno set to ENOMEM by ye_sbrk
    }
    set_block(hdr, PAGE_SIZE, 0);
    return hdr;
}

/*
 * Look for a block in the current free lists (before calling ye_sbrk)
 */
static ye_header *seg_find_current(size_t size) {
    ye_header *hdr;
    for (int i = seg_index(size); i < NUM_LISTS; i++) {
        hdr = seglist[i].head;
        for (hdr = seglist[i].head; hdr != NULL; hdr = hdr->next) {
            if (BLOCKSIZE(hdr) >= size) {
                return hdr;
            }
        }
    }
    return NULL;
}

ye_header *seg_find(size_t size) {
    ye_header *hdr, *pghdr;
    hdr = seg_find_current(size);
    if (hdr == NULL) {
        if (size > heap_max() - heap_min()) {  // avoid ye_sbrking the entire heap on a too-big request.
            errno = ENOMEM;
            return NULL;
        }
        do { // Didn't find a block of sufficient size, time to ye_sbrk a page at a time.
            if ((pghdr = add_page()) == NULL) {
                errno = ENOMEM;
                return NULL;
            }
            hdr = try_coalesce_backwards(pghdr);
        } while (BLOCKSIZE(hdr) < size);
    }
    seg_rm(hdr);
    try_split_coalesce_forwards(hdr, size); // Already rounded by ye_malloc.
    return hdr;
}
