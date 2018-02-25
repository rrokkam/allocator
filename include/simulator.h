#ifndef SIMULATOR_H
#define SIMULATOR_H

#define NM_PAGES 16  // Set >= 1 TODO: fix name
#define PAGE_SIZE 1024  // 1 KB

/*
 * Any program using the sfmm library must call this function ONCE
 * before issuing any allocation requests. This function DOES NOT
 * allocate any space to your allocator.
 */
void setup();

/*
 * Any program using the sfmm library must call this function ONCE
 * after all allocation requests are complete. If implemented cleanly,
 * your program should have no memory leaks in valgrind after this function
 * is called.
 */
void teardown();

/*
 * @return The starting address of the heap for your allocator.
 */
void *heap_min();

/*
 * @return The ending address of the heap for your allocator.
 */
void *heap_max();

/*
 * This function changes the position of your program's break.
 * Calling this function increments the break by 1 page and updates
 * the heap start and end variables, which can be accessed through
 * heap_min() and heap_max().
 *
 * @return On success, this function returns the previous program break.
 * If the break was increased, this value is a pointer to the start of the newly
 * allocated memory. On error, (void *) -1 is returned and errno is set to ENOMEM.
 */
void *ye_sbrk();

#endif