#ifndef HELPERS_H
#define HELPERS_H

#include "allocator.h"
#include <stdbool.h>
#include <stdlib.h>

#define NUM_PAGES 4
#define MIN_PAYLOAD_SIZE 16
#define YE_OVERHEAD (YE_HEADER_SIZE + YE_HEADER_SIZE)
#define MIN_BLOCK_SIZE (YE_OVERHEAD + MIN_PAYLOAD_SIZE)
#define MAX_BLOCK_SIZE ((2^16) - 1)  // TODO: replace with maximum uint16_t value

#define TERMINATE(no) {errno = no; return NULL;}

#define BLOCKSIZE(ptr) (((ye_header *)ptr)->block_size << 4)
#define FOOTER(ptr) ((void *)ptr + BLOCKSIZE(ptr) - YE_HEADER_SIZE)
#define HEADER(ptr) ((void *)ptr - BLOCKSIZE(ptr) + YE_HEADER_SIZE)
#define ALLOCATED(ptr) (((ye_header *)ptr)->allocated)
#define VALIDSIZE(size) ((size != 0) && ((uint16_t)size <= MAX_BLOCK_SIZE))
#define PAYLOADSIZE(size) (size - YE_OVERHEAD)

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
