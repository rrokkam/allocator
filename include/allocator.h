#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define BLOCKSIZE_BITS 28
#define UNUSED 3
#define ALLOCATED_BIT 1
#define REQSIZE_BITS 32

#define SF_HEADER_SIZE (REQSIZE_BITS + BLOCKSIZE_BITS + UNUSED + ALLOCATED_BIT)
#define SF_FOOTER_SIZE SF_HEADER_SIZE

#define LIST_1_MIN 32
#define LIST_1_MAX 128
#define LIST_2_MIN 129
#define LIST_2_MAX 512
#define LIST_3_MIN 513
#define LIST_3_MAX 2048
#define LIST_4_MIN 2049
#define LIST_4_MAX -1
#define PAGE_SZ 4096

#define FREE_LIST_COUNT 4

/* Struct for an allocated block header */
typedef struct ye_header {
    uint64_t      allocated : ALLOCATED_BIT;
    uint64_t         unused : UNUSED;
    uint64_t     block_size : BLOCKSIZE_BITS;
    uint64_t requested_size : REQSIZE_BITS;
    struct ye_header *next;
    struct ye_header *prev;
} __attribute__((packed)) ye_header;

/* Segregated free list struct */
typedef struct {
    ye_header *head;
    uint16_t min;
    uint16_t max;
} free_list;

/* This is the segregated free list */
extern free_list seg_free_list[FREE_LIST_COUNT];

/*
 * This is your implementation of ye_malloc. It acquires uninitialized memory that
 * is aligned and padded properly for the underlying system.
 *
 * @param size The number of bytes requested to be allocated.
 *
 * @return If successful, the pointer to a valid region of memory of the
 * requested size is returned, else NULL is returned and ye_errno as follows:
 *
 * If size is invalid (0 or greater than 4 pages), ye_errno is set to EINVAL
 * If the request cannot be satisfied, ye_errno is set to ENOMEM
 */
void *ye_malloc(size_t size);

/*
 * Resizes the memory pointed to by ptr to size bytes.
 *
 * @param ptr Address of the memory region to resize.
 * @param size The minimum size to resize the memory to.
 *
 * @return If successful, the pointer to a valid region of memory is
 * returned, else NULL is returned and ye_errno is set appropriately.
 *
 * If there is no memory available ye_realloc should set ye_errno to ENOMEM.
 * If ye_realloc is called with an invalid pointer ye_errno should be set to EINVAL.
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

#endif
