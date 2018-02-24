#include "debug.h"
#include "seglist.h"
#include "allocator.h"

free_list seg_free_list[4] = {
  {NULL, LIST_1_MIN, LIST_1_MAX},
  {NULL, LIST_2_MIN, LIST_2_MAX},
  {NULL, LIST_3_MIN, LIST_3_MAX},
  {NULL, LIST_4_MIN, LIST_4_MAX}
};

int seg_listindex(size_t size) {
    free_list list;
    for(int i = 0; i < FREE_LIST_COUNT - 1; i++) {
        list = seg_free_list[i];
        if((list.min <= size) && (size <= list.max)) return i;
    }
    return FREE_LIST_COUNT - 1;
}

/**
 * Will cause crash if called with null param.
 */
void seg_insert(ye_header *blockhdr) {
    int index = seg_listindex(BLOCKSIZE(blockhdr));
    ye_header *nexthdr = seg_free_list[index].head;
    seg_free_list[index].head = blockhdr;
    blockhdr->next = nexthdr;
    blockhdr->prev = NULL;
    if(nexthdr != NULL) nexthdr->prev = blockhdr;
}

/**
 * Makes no changes to the allocated, etc bits. Only removes
 * the free block from the seg_free_list.
 */
void seg_remove(ye_header *blockhdr) {
    int index = seg_listindex(BLOCKSIZE(blockhdr));
    ye_header *prevhdr = blockhdr->prev;
    ye_header *nexthdr = blockhdr->next;
    if(prevhdr != NULL) prevhdr->next = nexthdr;
    else seg_free_list[index].head = nexthdr;
    if(nexthdr != NULL) nexthdr->prev = prevhdr;
}

/**
 * NOTE - returns FREE_LIST_COUNT - 1 if the size is too small!
 * Keep in mind LIST_4_MAX = -1.
 */
int seg_listindex(size_t size) {
    free_list list;
    for(int i = 0; i < FREE_LIST_COUNT - 1; i++) {
        list = seg_free_list[i];
        if((list.min <= size) && (size <= list.max)) return i;
    }
    return FREE_LIST_COUNT - 1;
}

/**
 * Return NULL if there is no block of sufficient size.
 * Finds a block with BLOCKSIZE(block) >= size (in bytes).
 */
ye_header *seg_findspace(size_t size) {
    ye_header *blockhdr, *pghdr, *newhdr;
    for(int i = seg_listindex(size); i < FREE_LIST_COUNT; i++) {
        blockhdr = seg_free_list[i].head;
        while(blockhdr != NULL) {
            if(BLOCKSIZE(blockhdr) >= size) return blockhdr;
            blockhdr = blockhdr->next;
        }
    }

    do { // Didn't find a block of sufficient size, time to sbrk.
        if((pghdr = addpage()) == NULL) return NULL;
        /* newhdr = seg_insert(pghdr); */


        blockhdr = prevblock(pghdr); // try_coalesce_down
        if(blockhdr != NULL && !ALLOCATED(blockhdr)) { // handles before getheapstart
            seg_remove(blockhdr); // extra inserts and removes are a little silly
                                    // but are needed for coalesce to work properly.
            coalesce(blockhdr, pghdr);
            seg_insert(blockhdr);
            newhdr = blockhdr;
        } else {
            seg_insert(pghdr);
            newhdr = pghdr;
        }
    } while (BLOCKSIZE(newhdr) < size); // don't need to check the rest, this is biggest
    return newhdr;
}
