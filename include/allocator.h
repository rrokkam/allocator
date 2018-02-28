#ifndef ALLOCATOR_H
#define ALLOCATOR_H

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
 * TODO: Documentation of calloc
 */
void *ye_calloc(size_t nmemb, size_t size);

#endif /* ALLOCATOR_H */
