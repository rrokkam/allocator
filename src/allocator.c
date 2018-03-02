#include "allocator.h"
#include "debug.h"

#include <string.h>

#include "blocks.h"
#include "segfreelist.h"

void *ye_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    } 
    size_t rsize = reqsize(size);
    ye_header *hdr = seg_find(rsize);
    if (hdr == NULL) {
        return NULL;
    }
    return PAYLOAD(hdr);  // points right after the header (to the payload)
}

void ye_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    ye_header *hdr = HEADER(ptr);
    if (!ALLOCATED(hdr)) {
        error("Double free or corruption occurred at %p.", ptr);
        abort();
    }
    try_coalesce_bidir(hdr);
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
    if (ptr == NULL) {
        return ye_malloc(size);
    }
    if (size == 0) {
        ye_free(ptr);
        return NULL;
    }
    ye_header *hdr = HEADER(ptr);
    size_t bsize = BLOCKSIZE(hdr);
    size_t rsize = reqsize(size);
    if (bsize < rsize) { // upsizing
        ye_header *nexthdr = nextblock(hdr);
        if (bsize + BLOCKSIZE(nexthdr) >= rsize) {
            try_coalesce_forwards(hdr);
        } else { // need to move to another location
            void *newptr = ye_malloc(rsize); // get a well-fitting block of memory
            if (newptr == NULL) {
                return NULL;
            }
            memcpy(newptr, ptr, size); // won't copy cruft bytes at the end
            ye_free(ptr);
            return newptr;
        }
    } else if (bsize > rsize) { // downsizing
        try_split_coalesce_forwards(hdr, rsize);
    }
    return ptr; // if bsize == size, NOOP. If the memory isn't moved, fall through.
}
