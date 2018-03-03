#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>  // for size_t

/* Documentation from this file is adapted from glibc's malloc docs. */

/*
 * Allocates size bytes of memory. The memory is not initialized. 
 * The pointer returned is double-word aligned. 
 * 
 * If size is 0, then ye_malloc returns NULL. If there is not enough memory
 * to satisfy the request, ye_malloc returns NULL and sets errno to ENOMEM.
 */
void *ye_malloc(size_t size);

/*
 * Frees the memory space pointed to by ptr, which must have been
 * returned by a previous call to ye_malloc, ye_calloc, or ye_realloc.
 *
 * If ptr was not returned by a such call, or if ye_free(ptr) was previously
 * called, then the result of ye_free is undefined. If ptr is NULL, then
 * no operation is performed.
 */
void ye_free(void *ptr);

/*
 * Allocates memory for an array of nmemb objects of size bytes each.
 * The memory is initialized to 0.
 *
 * If either nmemb or size is 0, then ye_calloc returns NULL. If there is 
 * not enough memory to satisfy the request, ye_calloc returns NULL and
 * sets errno to ENOMEM.
 */
void *ye_calloc(size_t nmemb, size_t size);

/*
 * Resizes the memory pointed to by ptr to size bytes. The contents of
 * the memory region between the start of the region and the minimum of
 * the old and new sizes. 
 *
 * If the new size is larger than the current size, the new memory will
 * not be initialized. 
 *
 * If ptr is NULL, then ye_realloc is equivalent to ye_malloc(size). If size 
 * is 0 and ptr is not NULL, then ye_realloc is equivalent to ye_free(ptr). 
 * 
 * Unless ptr is NULL, it must have been returned by an earlier call to 
 * ye_malloc(), ye_calloc() or ye_realloc(). If the area pointed to was 
 * moved, a free(ptr) is done.
 *
 * If there is not enough memory to satisfy the request, ye_malloc returns
 * NULL and sets errno to ENOMEM.
 */
void *ye_realloc(void *ptr, size_t size);

#endif /* ALLOCATOR_H */
