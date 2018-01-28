#ifndef HELPERS_H
#define HELPERS_H

#include "allocator.h"
#include <stdlib.h>

#define NUM_PAGES 4
#define MIN_PAYLOAD_SIZE 16
#define SF_HEADER_SIZE_BYTES (SF_HEADER_SIZE >> 3)
#define SF_FOOTER_SIZE_BYTES (SF_FOOTER_SIZE >> 3)
#define SF_OVERHEAD (SF_HEADER_SIZE_BYTES + SF_FOOTER_SIZE_BYTES)
#define MIN_BLOCK_SIZE (SF_OVERHEAD + MIN_PAYLOAD_SIZE)
#define MAX_BLOCK_SIZE (PAGE_SZ * NUM_PAGES)

#define TERMINATE(no) {sf_errno = no; return NULL;}

#define BLOCKSIZE(ptr) (((sf_header *)ptr)->block_size << 4)
#define FOOTER(ptr) ((void *)ptr + BLOCKSIZE(ptr) - SF_FOOTER_SIZE_BYTES)
#define HEADER(ptr) ((void *)ptr - BLOCKSIZE(ptr) + SF_FOOTER_SIZE_BYTES)
#define ALLOCATED(ptr) (((sf_header *)ptr)->allocated)
#define VALIDSIZE(size) ((size != 0) && ((size_t)size <= NUM_PAGES * PAGE_SZ))
#define PAYLOADSIZE(size) (size - SF_OVERHEAD)

void prepare(void *blockhdr, size_t requested_size,
    size_t block_size, bool allocated);

void coalesce(void *firsthdr, void *secondhdr);

void try_coalesce_up(void *blockhdr);

size_t required_size(size_t size);

bool can_split(void *blockhdr, size_t size);

void *split(void *blockhdr, size_t size);

bool valid_block(void *ptr);

void *addpage();

void *nextblock(void *blockhdr);

void *prevblock(void *blockhdr);


#endif /* HELPERS_H */