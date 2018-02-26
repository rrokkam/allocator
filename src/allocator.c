#include "debug.h"
#include "utils.h"
#include "allocator.h"
#include "segfreelist.h"
#include <stdint.h>
#include <string.h>
#include <errno.h>

void *ye_malloc(size_t size) {
    ye_header *blockhdr = seg_find(size);
    if (blockhdr == NULL) {
        return NULL;
    }

    if(can_split(blockhdr, size)) {
        ye_header *newhdr = split(blockhdr, size); // prepares the split off block
        // TODO: coalesce newhdr with next if possible
        seg_add(newhdr);
    }
    prepare(blockhdr, size, 1);

    return ((void *) blockhdr) + YE_HEADER_SIZE;
}

void *ye_realloc(void *ptr, size_t size) {
    ye_header *blockhdr = ptr - YE_HEADER_SIZE;
    if (size == 0) {
        ye_free(ptr);
        return NULL;
    } // size too big case is covered by valid_block and malloc returning null

    size_t reqsize = required_size(size);
    size_t blocksize = BLKSIZE(blockhdr);
    if (blocksize < reqsize) { // upsize
        void *newptr = ye_malloc(size); // round up to 16, split done in ye_malloc
        if (newptr == NULL) {
            return NULL;
        }
        memcpy(newptr, ptr, size);
        ye_free(ptr);
        return newptr;
    } else if (blocksize > reqsize) { // downsize
        if (can_split(blockhdr, reqsize)) { // repeated code with free..
            ye_header *newhdr = split(blockhdr, reqsize);
            try_coalesce_up(newhdr);
            prepare(blockhdr, size, 1);
        }
    }
    return ptr;
}

void ye_free(void *ptr) {
    ye_header *blockhdr = ptr - YE_HEADER_SIZE;
    if (!ALLOCATED(blockhdr)) {
        error("Attempted to free non-allocated memory.");
        abort();
    }
    try_coalesce_up(blockhdr);
    blockhdr->alloc = 0;
    ((ye_header *)FOOTER(blockhdr))->alloc = 0;
}

void *ye_calloc(size_t nmemb, size_t size) {
    size_t len = nmemb * size;
    void *ret = ye_malloc(len);
    if (ret == NULL) {  // memset segfaults on ret = NULL
        return NULL;
    }
    return memset(ret, 0, len);
}
