#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define BLOCKSIZE_BITS 28
#define UNUSED 3
#define ALLOCATED_BIT 1

#define YE_HEADER_SIZE (2 * (BLOCKSIZE_BITS + UNUSED + ALLOCATED_BIT))  // 64

// TODO: change names to next and prev
typedef struct ye_header {
    uint64_t      allocated : ALLOCATED_BIT;
    uint64_t         unused : UNUSED;
    uint64_t     block_size : BLOCKSIZE_BITS;
    uint64_t     prev_alloc : ALLOCATED_BIT;
    uint64_t    prev_unused : UNUSED;
    uint64_t requested_size : BLOCKSIZE_BITS;
    struct ye_header *next;
    struct ye_header *prev;
} __attribute__((packed)) ye_header;
    
/*
 * Acquires uninitialized memory that is aligned properly.
 *
 * @param size The number of bytes requested to be allocated.
 *
 * @return If successful, the pointer to a valid region of memory of the
 * requested size is returned, else NULL is returned and errno as follows:
 *
 * If size is invalid (0 or greater than 4 pages), errno is set to EINVAL
 * If the request cannot be satisfied, errno is set to ENOMEM
 */
void *ye_malloc(size_t size);

/*
 * Resizes the memory pointed to by ptr to size bytes.
 *
 * @param ptr Address of the memory region to resize.
 * @param size The minimum size to resize the memory to.
 *
 * @return If successful, the pointer to a valid region of memory is
 * returned, else NULL is returned and errno is set appropriately.
 *
 * If there is no memory available ye_realloc should set errno to ENOMEM.
 * If ye_realloc is called with an invalid pointer errno should be set to EINVAL.
 *
 * If ye_realloc is called with a valid pointer and a size of 0 it should free
 * the allocated block and return NULL.
 */
void *ye_realloc(void *ptr, size_t size);

/*
 * Marks a dynamically allocated region as no longer in use.
 * Adds the newly freed block to the free list.
 *
 * @param ptr Address of memory returned by the function ye_malloc.
 *
 * If ptr is invalid, the function calls abort() to exit the program.
 */
void ye_free(void *ptr);

/*
 * TODO: Documentation of realloc
 */
void *ye_realloc(void *ptr, size_t size);

#endif /* ALLOCATOR_H */
