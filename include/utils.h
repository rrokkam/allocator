#ifndef UTILS_H
#define UTILS_H

#include "allocator.h"
#include "segfreelist.h"
#include <stdbool.h>
#include <stdlib.h>

#define MIN_PAYLOAD_SIZE 16
#define YE_OVERHEAD (YE_HEADER_SIZE + YE_HEADER_SIZE)

#define TERMINATE(no) {errno = no; return NULL;}

#define BLKSIZE(ptr) (((ye_header *)ptr)->size << 4)
#define FOOTER(ptr) ((void *)ptr + BLKSIZE(ptr) - YE_HEADER_SIZE)
#define HEADER(ptr) ((void *)ptr - BLKSIZE(ptr) + YE_HEADER_SIZE)
#define ALLOCATED(ptr) (((ye_header *)ptr)->alloc)
#define VALIDSIZE(size) ((size != 0) && ((uint16_t)size <= MAX_BLOCK_SIZE))
#define PAYLOADSIZE(size) (size - YE_OVERHEAD)

void prepare(ye_header *hdr, size_t rsize, bool alloc);

void *addpage();

ye_header *split(ye_header *hdr, size_t rsize);

bool try_coalesce_next(ye_header *hdr);

bool try_coalesce_prev(ye_header *hdr);

#endif /* UTILS_H */
