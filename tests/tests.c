// #include <criterion/criterion.h>
// #include <errno.h>
// #include <signal.h>
// #include "allocator.h"
// #include "helpers.h"
// #include "seglist.h"
// #include <stdio.h>

// int find_list_index_from_size(int sz) {
//  if (sz >= LIST_1_MIN && sz <= LIST_1_MAX) return 0;
//  else if (sz >= LIST_2_MIN && sz <= LIST_2_MAX) return 1;
//  else if (sz >= LIST_3_MIN && sz <= LIST_3_MAX) return 2;
//  else return 3;
// }

// Test(ye_memsuite_student, Malloc_an_Integer_check_freelist, .init = ye_mem_init, .fini = ye_mem_fini) {
//  errno = 0;
//  int *x = ye_malloc(sizeof(int));

//  cr_assert_not_null(x);

//  *x = 4;

//  cr_assert(*x == 4, "ye_malloc failed to give proper space for an int!");

//  ye_header *header = (ye_header*)((char*)x - 8);

//  /* There should be one block of size 4064 in list 3 */
//  free_list *fl = &seg_free_list[find_list_index_from_size(PAGE_SIZE - (header->block_size << 4))];

//  cr_assert_not_null(fl, "Free list is null");

//  cr_assert_not_null(fl->head, "No block in expected free list!");
//  cr_assert_null(fl->head->next, "Found more blocks than expected!");
//  cr_assert(fl->head->header.block_size << 4 == 4064);
//  cr_assert(fl->head->header.allocated == 0);
//  cr_assert(errno == 0, "errno is not zero!");
//  cr_assert(get_heap_start() + PAGE_SIZE == get_heap_end(), "Allocated more than necessary!");
// }

// Test(ye_memsuite_student, Malloc_over_four_pages, .init = ye_mem_init, .fini = ye_mem_fini) {
//  errno = 0;
//  void *x = ye_malloc(PAGE_SIZE << 2);

//  cr_assert_null(x, "x is not NULL!");
//  cr_assert(errno == ENOMEM, "errno is not ENOMEM!");
// }

// Test(ye_memsuite_student, free_double_free, .init = ye_mem_init, .fini = ye_mem_fini, .signal = SIGABRT) {
//  errno = 0;
//  void *x = ye_malloc(sizeof(int));
//  ye_free(x);
//  ye_free(x);
// }

// Test(ye_memsuite_student, free_no_coalesce, .init = ye_mem_init, .fini = ye_mem_fini) {
//  errno = 0;
//  /* void *x = */ ye_malloc(sizeof(long));
//  void *y = ye_malloc(sizeof(double) * 10);
//  /* void *z = */ ye_malloc(sizeof(char));

//  ye_free(y);

//  free_list *fl = &seg_free_list[find_list_index_from_size(96)];

//  cr_assert_not_null(fl->head, "No block in expected free list");
//  cr_assert_null(fl->head->next, "Found more blocks than expected!");
//  cr_assert(fl->head->header.block_size << 4 == 96);
//  cr_assert(fl->head->header.allocated == 0);
//  cr_assert(errno == 0, "errno is not zero!");
// }

// Test(ye_memsuite_student, free_coalesce, .init = ye_mem_init, .fini = ye_mem_fini) {
//  errno = 0;
//  /* void *w = */ ye_malloc(sizeof(long));
//  void *x = ye_malloc(sizeof(double) * 11);
//  void *y = ye_malloc(sizeof(char));
//  /* void *z = */ ye_malloc(sizeof(int));

//  ye_free(y);
//  ye_free(x);

//  free_list *fl_y = &seg_free_list[find_list_index_from_size(32)];
//  free_list *fl_x = &seg_free_list[find_list_index_from_size(144)];

//  cr_assert_null(fl_y->head, "Unexpected block in list!");
//  cr_assert_not_null(fl_x->head, "No block in expected free list");
//  cr_assert_null(fl_x->head->next, "Found more blocks than expected!");
//  cr_assert(fl_x->head->header.block_size << 4 == 144);
//  cr_assert(fl_x->head->header.allocated == 0);
//  cr_assert(errno == 0, "errno is not zero!");
// }

// Test(ye_memsuite_student, freelist, .init = ye_mem_init, .fini = ye_mem_fini) {
//  /* void *u = */ ye_malloc(1);          //32
//  void *v = ye_malloc(LIST_1_MIN); //48
//  void *w = ye_malloc(LIST_2_MIN); //160
//  void *x = ye_malloc(LIST_3_MIN); //544
//  void *y = ye_malloc(LIST_4_MIN); //2080
//  /* void *z = */ ye_malloc(1); // 32

//  int allocated_block_size[4] = {48, 160, 544, 2080};

//  ye_free(v);
//  ye_free(w);
//  ye_free(x);
//  ye_free(y);

//  // First block in each list should be the most recently freed block
//  for (int i = 0; i < FREE_LIST_COUNT; i++) {
//      ye_header *fh = (ye_header *)(seg_free_list[i].head);
//      cr_assert_not_null(fh, "list %d is NULL!", i);
//      cr_assert(fh->header.block_size << 4 == allocated_block_size[i], "Unexpected free block size!");
//      cr_assert(fh->header.allocated == 0, "Allocated bit is set!");
//  }

//  // There should be one free block in each list, 2 blocks in list 3 of size 544 and 1232
//  for (int i = 0; i < FREE_LIST_COUNT; i++) {
//      ye_header *fh = (ye_header *)(seg_free_list[i].head);
//      if (i != 2)
//          cr_assert_null(fh->next, "More than 1 block in freelist [%d]!", i);
//      else {
//          cr_assert_not_null(fh->next, "Less than 2 blocks in freelist [%d]!", i);
//          cr_assert_null(fh->next->next, "More than 2 blocks in freelist [%d]!", i);
//      }
//  }
// }

// Test(ye_memsuite_student, realloc_larger_block, .init = ye_mem_init, .fini = ye_mem_fini) {
//  void *x = ye_malloc(sizeof(int));
//  /* void *y = */ ye_malloc(10);
//  x = ye_realloc(x, sizeof(int) * 10);

//  free_list *fl = &seg_free_list[find_list_index_from_size(32)];

//  cr_assert_not_null(x, "x is NULL!");
//  cr_assert_not_null(fl->head, "No block in expected free list!");
//  cr_assert(fl->head->header.block_size << 4 == 32, "Free Block size not what was expected!");

//  ye_header *header = (ye_header*)((char*)x - 8);
//  cr_assert(header->block_size << 4 == 64, "Realloc'ed block size not what was expected!");
//  cr_assert(header->allocated == 1, "Allocated bit is not set!");
// }

// Test(ye_memsuite_student, realloc_smaller_block_splinter, .init = ye_mem_init, .fini = ye_mem_fini) {
//  void *x = ye_malloc(sizeof(int) * 8);
//  void *y = ye_realloc(x, sizeof(char));

//  cr_assert_not_null(y, "y is NULL!");
//  cr_assert(x == y, "Payload addresses are different!");

//  ye_header *header = (ye_header*)((char*)y - 8);
//  cr_assert(header->allocated == 1, "Allocated bit is not set!");
//  cr_assert(header->block_size << 4 == 48, "Block size not what was expected!");

//  free_list *fl = &seg_free_list[find_list_index_from_size(4048)];

//  // There should be only one free block of size 4048 in list 3
//  cr_assert_not_null(fl->head, "No block in expected free list!");
//  cr_assert(fl->head->header.allocated == 0, "Allocated bit is set!");
//  cr_assert(fl->head->header.block_size << 4 == 4048, "Free block size not what was expected!");
// }

// Test(ye_memsuite_student, realloc_smaller_block_free_block, .init = ye_mem_init, .fini = ye_mem_fini) {
//  void *x = ye_malloc(sizeof(double) * 8);
//  void *y = ye_realloc(x, sizeof(int));

//  cr_assert_not_null(y, "y is NULL!");

//  ye_header *header = (ye_header*)((char*)y - 8);
//  cr_assert(header->block_size << 4 == 32, "Realloc'ed block size not what was expected!");
//  cr_assert(header->allocated == 1, "Allocated bit is not set!");


//  // After realloc'ing x, we can return a block of size 48 to the freelist.
//  // This block will coalesce with the block of size 4016.
//  free_list *fl = &seg_free_list[find_list_index_from_size(4064)];

//  cr_assert_not_null(fl->head, "No block in expected free list!");
//  cr_assert(fl->head->header.allocated == 0, "Allocated bit is set!");
//  cr_assert(fl->head->header.block_size << 4 == 4064, "Free block size not what was expected!");
// }


// //############################################
// //STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
// //DO NOT DELETE THESE COMMENTS
// //############################################

// // This test depends on the values of the list 1 - 4 mins and maxes.
// Test(ye_memsuite_student, list_index, .init = ye_mem_init, .fini = ye_mem_fini) {
//  cr_assert(seg_listindex(LIST_2_MIN) == find_list_index_from_size(LIST_2_MIN),
//      "List 2 min not placed in list 2");
//  cr_assert(seg_listindex(LIST_2_MIN - 1) == 0,
//      "Number less than List 2 min not placed in list 1");
//  cr_assert(seg_listindex(PAGE_SIZE << 1) == 3, "2048 not in list 4");
// }

// Test(ye_memsuite_student, required_block_size, .init = ye_mem_init, .fini = ye_mem_fini) {
//  cr_assert(required_size(2) == 32, "Required size less than 32");
//  cr_assert(required_size(48) == 64, "Required size of perfectly fitting block is wrong");
//  cr_assert(required_size(47) == 64, "Required size of odd-sized block not padded");
//  cr_assert(required_size(1600) == 1616, "Required size of large number given incorrectly");
// }

// Test(ye_memsuite_student, nextblock_prevblock, .init = ye_mem_init, .fini = ye_mem_fini) {
//  void *x = ye_malloc(64);
//  void *y = ye_malloc(96);
//  ye_header *xhdr = (ye_header *)(x - 8);
//  ye_header *yhdr = (ye_header *)(y - 8);

//  cr_assert_not_null(x, "x is NULL!");
//  cr_assert_not_null(xhdr, "xhdr is NULL!");
//  cr_assert_not_null(y, "y is NULL!");
//  cr_assert_not_null(yhdr, "yhdr is NULL!");

//  cr_assert(nextblock(xhdr) == yhdr, "Next block of alloc'd block not correctly computed");
//  cr_assert(prevblock(yhdr) == xhdr, "Previous block of alloc'd block not correctly computed");
//  ye_free(x);
//  cr_assert(nextblock(xhdr) == yhdr, "Next block of free block not correctly computed");
//  x = ye_malloc(64);
//  ye_free(y);
//  cr_assert(prevblock(yhdr) == xhdr, "Previous block of free block not correctly computed");
// }

// Test(ye_memsuite_student, malloc_overlap_pages, .init = ye_mem_init, .fini = ye_mem_fini) {
//  void *x = ye_malloc(sizeof(int));
//  void *y = ye_malloc(PAGE_SIZE);

//  cr_assert_not_null(x, "x is NULL!");
//  cr_assert_not_null(y, "y is NULL!");

//  cr_assert(errno == 0, "Unexpected error given");
//  ye_header *header = (ye_header *)((char *)y - 8);
//  cr_assert(header->block_size << 4 == PAGE_SIZE + 16, "Block size was not what was expected!");
//  cr_assert(header->allocated == 1, "Allocated bit is not set!");
// //   cr_assert(header->padded == 0, "Padded bit is set!");

//  cr_assert(get_heap_end() - get_heap_start() == 2*PAGE_SIZE, "Sbrk not called twice!");

// }

// Test(ye_memsuite_student, realloc_same_size_zero, .init = ye_mem_init, .fini = ye_mem_fini) {
//  long *x = ye_malloc(sizeof(long) * 2);
//  x[0] = 12345;
//  x[1] = 67890;

//  x = ye_realloc(x, sizeof(long) * 2);

//  cr_assert_not_null(x, "x is NULL!");
//  cr_assert(x[0] == 12345 && x[1] == 67890, "Realloc didn't preserve the payload!");

//  x = ye_realloc(x, 0);

//  cr_assert_null(x, "Realloc to zero didn't return NULL pointer!");
// }
