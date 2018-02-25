#ifndef HELPERS_H
#define HELPERS_H

#include "allocator.h"
#include "segfreelist.h"
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

/*
 * Function which outputs the state of the free-lists to stderr.
 * Performs checks on the placement of the header and footer
 * See ye_snapshot section for details on output format.
 */
void ye_snapshot();

/*
 * Function which prints human readable block format
 *
 * Note: If there are 'bad' addresses in your free list
 * this function will most likely segfault.
 *
 * @param block Address of the block header in memory.
 */
void ye_blockprint(ye_header *block);

/*
 * Prints human readable block format from the address of the payload.
 * IE. subtracts header size from the data pointer to obtain the address
 * of the block header. Calls ye_blockprint internally to print.
 *
 * Note: If there are 'bad' addresses in your free list
 * this function will most likely segfault.
 *
 * @param data Pointer to payload data in memory
 * (value returned by ye_malloc).
 */
void ye_varprint(void *data);


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
