//#include "newallocator.h"
#include "debug.h"

#include <string.h>

#include "segfreelist.h"

/* Round size up to the nearest multiple of 8 */
#define ROUND(size) (((size_t) size & ~0x07) + 8 * (((size_t) size & 0x07) != 0))

/* Get the header of a block from a payload pointer */
#define HEADER(payload) ((void *) payload - sizeof(ye_header))

/* Evaluate to 1 if allocated, and 0 if free */
#define ALLOCATED(hdr) (((ye_header *) hdr)->alloc)

/* */
#define BLOCKSIZE(ptr) (((ye_header *)ptr)->size << 4)

void *ye_malloc(size_t size) {
    size_t rsize = ROUND(size);
    ye_header *hdr = seg_find(rsize);
    if (hdr == NULL) {
        return NULL;
    }
    return hdr++;  // points right after the header (to the payload)
}

void ye_free(void *ptr) {
    ye_header *hdr = HEADER(ptr);
    if (!ALLOCATED(hdr)) {
        error("Double free or corruption occurred at %p.", ptr);
        abort();
    }
    try_coalesce_insert(hdr);
}

void *ye_calloc(size_t nmemb, size_t size) {
    size_t len = nmemb * size;
    void *ptr = ye_malloc(len);  // pointer to payload
    if (ptr == NULL) {  // memset segfaults on ret = NULL
        return NULL;
    }
    memset(ptr, 0, len);
    return ptr;
}

void *ye_realloc(void *ptr, size_t size) {
    if (size == 0) {
        ye_free(ptr);
        return NULL;
    }
    ye_header *hdr = HEADER(ptr);
    size_t bsize = BLOCKSIZE(hdr);
    if (bsize < size) { // upsizing
        ye_header *nexthdr = nextblock(hdr);
        if (!ALLOCATED(nexthdr) && (bsize + BLOCKSIZE(nexthdr) >= size)) {
            coalesce_forwards(hdr, nexthdr);
        } else { // need to move to another location
            void *newptr = ye_malloc(size); // get a well-fitting block of memory
            if (newptr == NULL) {
                return NULL;
            }
            memcpy(newptr, ptr, size); // won't copy cruft bytes at the end
            ye_free(ptr);
            return newptr;
        }
    } else if (bsize > size) { // downsizing
        try_split_coalesce_forwards(hdr, size);
    }
    return ptr; // if blocksize == size, we do nothing. If downsizing, fall through.
}
