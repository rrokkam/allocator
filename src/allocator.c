#include "debug.h"
#include "helpers.h"
#include "allocator.h"
#include "seglist.h"
#include <string.h>
#include <errno.h>

int sf_errno = 0;

void *sf_malloc(size_t size) {
    if(!VALIDSIZE(size)) TERMINATE(EINVAL);
    size_t reqsize = required_size(size);
    sf_header *blockhdr = seg_findspace(reqsize);
    if(blockhdr == NULL) TERMINATE(ENOMEM);
    seg_remove(blockhdr);

    if(can_split(blockhdr, reqsize)) {
        sf_header *newhdr = split(blockhdr, reqsize); // prepares split block
        // no coalesce :( otherwise could have removed repeated code
        seg_insert(newhdr);
    }
    size_t blocksize = BLOCKSIZE(blockhdr); // size depends on previous branch.
    prepare(blockhdr, size, blocksize, 1);

    return ((void *) blockhdr) + SF_HEADER_SIZE_BYTES;
}

void *sf_realloc(void *ptr, size_t size) {
    sf_header *blockhdr = ptr - SF_HEADER_SIZE_BYTES;
    if(!valid_block(blockhdr)) abort();
    if(size == 0) {
        sf_free(ptr);
        return NULL;
    } // size too big case is covered by valid_block and malloc returning null

    size_t reqsize = required_size(size);
    size_t blocksize = BLOCKSIZE(blockhdr);
    if(blocksize < reqsize) { // upsize
        void *newptr = sf_malloc(size); // round up to 16, split done in sf_malloc
        if(newptr == NULL) return NULL;
        memcpy(newptr, ptr, size);
        sf_free(ptr);
        return newptr;
    } else if(blocksize > reqsize) { // downsize
        if(can_split(blockhdr, reqsize)) { // repeated code with free..
            sf_header *newhdr = split(blockhdr, reqsize);
            try_coalesce_up(newhdr);
            prepare(blockhdr, size, reqsize, 1);
        }
    }
    return ptr;
}

void sf_free(void *ptr) {
    sf_header *blockhdr = ptr - SF_HEADER_SIZE_BYTES;
    if(!valid_block(blockhdr) || !ALLOCATED(blockhdr)) abort();
    try_coalesce_up(blockhdr);
    blockhdr->allocated = 0; // rather than prepare 0 everything..
    ((sf_header *)FOOTER(blockhdr))->allocated = 0;
}
