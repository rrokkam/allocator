#include "debug.h"
#include "utils.h"
#include "allocator.h"
#include "segfreelist.h"
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define SPLITTABLE(hdr, size) ((BLKSIZE(hdr) - (size_t) size) >= MIN_BLOCK_SIZE)

// round up to the nearest multiple of 8
#define ROUND(size) ((((size_t) size & ~0x07) + 8 * (((size_t) size & 0x07) != 0)))

#define HDR(payload) ((void *) payload - sizeof(ye_header))

void *ye_malloc(size_t size) {
    size_t rsize = ROUND(size);
    ye_header *hdr = seg_find(rsize);
    if (hdr == NULL) {
        return NULL;
    }
    return hdr++;  // points right after the header (to the payload)
}

// TODO: this belongs in seg_find
    // if(SPLITTABLE(hdr, rsize)) {
    //     ye_header *newhdr = split(hdr, rsize); // prepares the split off block
    //     // TODO: coalesce newhdr with next if possible
    //     seg_add(newhdr);
    // }
    // prepare(hdr, rsize, 1);

    //return ((void *) hdr) + YE_HEADER_SIZE;

void *ye_realloc(void *ptr, size_t size) {
    if (size == 0) {
        ye_free(ptr);
        return NULL;
    }

    ye_header *hdr = HDR(ptr);

    // need to round up
    size_t blocksize = BLKSIZE(hdr);
    if (blocksize < size) { // upsize
        void *newptr = ye_malloc(size); // round up to 16, split done in ye_malloc
        if (newptr == NULL) {
            return NULL;
        }
        memcpy(newptr, ptr, size);
        ye_free(ptr);
        return newptr;
    } else if (blocksize > size) { // downsize
        if (SPLITTABLE(hdr, size)) { // repeated code with free..
            ye_header *newhdr = split(hdr, size);
            try_coalesce_next(newhdr);
            prepare(hdr, size, 1);
        }
    }
    return ptr;
}

void ye_free(void *ptr) {
    ye_header *hdr = ptr - sizeof(ye_header);
    if (!ALLOCATED(hdr)) {
        error("Attempted to free non-allocated memory.");
        abort();
    }
    try_coalesce(hdr);
    hdr->alloc = 0;
    ((ye_header *) FOOTER(hdr))->alloc = 0;
}

void *ye_calloc(size_t nmemb, size_t size) {
    size_t len = nmemb * size;
    void *ret = ye_malloc(len);
    if (ret == NULL) {  // memset segfaults on ret = NULL
        return NULL;
    }
    return memset(ret, 0, len);
}
