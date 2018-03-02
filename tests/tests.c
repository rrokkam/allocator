#include <criterion/criterion.h>
#include <errno.h>
#include <signal.h>
#include "allocator.h"
#include "blocks.h"
#include "segfreelist.h"
#include "simulator.h"
#include <stdio.h>

/* Placeholders */
#define LIST_1_MIN 16
#define LIST_1_MAX 23
#define LIST_2_MIN 24
#define LIST_2_MAX 31
#define LIST_3_MIN 32
#define LIST_3_MAX 39
#define LIST_4_MIN 40
#define LIST_4_MAX 47

int find_list_index_from_size(int sz) {
	if (sz >= LIST_1_MIN && sz <= LIST_1_MAX) return 0;
	else if (sz >= LIST_2_MIN && sz <= LIST_2_MAX) return 1;
	else if (sz >= LIST_3_MIN && sz <= LIST_3_MAX) return 2;
	else return 3;
}

Test(ye_memsuite, Malloc_an_Integer_check_freelist, .init = setup, .fini = teardown) {
	errno = 0;
	int *x = ye_malloc(sizeof(int));

	cr_assert_not_null(x);

	*x = 4;

	cr_assert(*x == 4, "ye_malloc failed to give proper space for an int!");

	ye_header *header = (ye_header *)((char*) x - 8);

	/* There should be one block of size 4064 in the last list */
	freelist *fl = &seglist[find_list_index_from_size(PAGE_SIZE - (header->size << 4))];

	cr_assert_not_null(fl, "Free list is null");

	cr_assert_not_null(fl->head, "No block in expected free list!");
	cr_assert_null(fl->head->next, "Found more blocks than expected!");
	cr_assert(fl->head->size << 4 == 4064);
	cr_assert(fl->head->alloc == 0);
	cr_assert(errno == 0, "errno is not zero!");
	cr_assert(heap_min() + PAGE_SIZE == heap_max(), "Allocated more than necessary!");
}

Test(ye_memsuite, Malloc_over_four_pages, .init = setup, .fini = teardown) {
	errno = 0;
	void *x = ye_malloc(PAGE_SIZE << 2);

	cr_assert_null(x, "x is not NULL!");
	cr_assert(errno == ENOMEM, "errno is not ENOMEM!");
}

Test(ye_memsuite, free_double_free, .init = setup, .fini = teardown, .signal = SIGABRT) {
	errno = 0;
	void *x = ye_malloc(sizeof(int));
	ye_free(x);
	ye_free(x);
}

Test(ye_memsuite, free_no_coalesce, .init = setup, .fini = teardown) {
	errno = 0;
	/* void *x = */ ye_malloc(sizeof(long));
	void *y = ye_malloc(sizeof(double) * 10);
	/* void *z = */ ye_malloc(sizeof(char));

	ye_free(y);

	freelist *fl = &seglist[find_list_index_from_size(96)];

	cr_assert_not_null(fl->head, "No block in expected free list");
	cr_assert_null(fl->head->next, "Found more blocks than expected!");
	cr_assert(fl->head->size << 4 == 96);
 	cr_assert(fl->head->alloc == 0);
	cr_assert(errno == 0, "errno is not zero!");
}

Test(ye_memsuite, free_coalesce, .init = setup, .fini = teardown) {
	errno = 0;
	/* void *w = */ ye_malloc(sizeof(long));
	void *x = ye_malloc(sizeof(double) * 11);
	void *y = ye_malloc(sizeof(char));
	/* void *z = */ ye_malloc(sizeof(int));
	
	ye_free(y);
	ye_free(x);

	freelist *fl_y = &seglist[find_list_index_from_size(32)];
	freelist *fl_x = &seglist[find_list_index_from_size(144)];

	cr_assert_null(fl_y->head, "Unexpected block in list!");
	cr_assert_not_null(fl_x->head, "No block in expected free list");
	cr_assert_null(fl_x->head->next, "Found more blocks than expected!");
	cr_assert(fl_x->head->size << 4 == 144);
	cr_assert(fl_x->head->alloc == 0);
	cr_assert(errno == 0, "errno is not zero!");
}

Test(ye_memsuite, freelist, .init = setup, .fini = teardown) {
	/* void *u = */ ye_malloc(1);    //32
	void *v = ye_malloc(LIST_1_MIN); //48
	void *w = ye_malloc(LIST_2_MIN); //160
	void *x = ye_malloc(LIST_3_MIN); //544
	void *y = ye_malloc(LIST_4_MIN); //2080
 /* void *z = */ ye_malloc(1); // 32

 int allocated_block_size[4] = {48, 160, 544, 2080};

 ye_free(v);
 ye_free(w);
 ye_free(x);
 ye_free(y);

 // First block in each list should be the most recently freed block
 for (int i = 0; i < NUM_LISTS; i++) {
	 ye_header *fh = (ye_header *)(seglist[i].head);
	 cr_assert_not_null(fh, "list %d is NULL!", i);
	 cr_assert(fh->size << 4 == allocated_block_size[i], "Unexpected free block size!");
	 cr_assert(fh->alloc == 0, "Allocated bit is set!");
 }

 // There should be one free block in each list, 2 blocks in list 3 of size 544 and 1232
 for (int i = 0; i < NUM_LISTS; i++) {
	 ye_header *fh = (ye_header *)(seglist[i].head);
	 if (i != 2)
		 cr_assert_null(fh->next, "More than 1 block in freelist [%d]!", i);
	 else {
		 cr_assert_not_null(fh->next, "Less than 2 blocks in freelist [%d]!", i);
		 cr_assert_null(fh->next->next, "More than 2 blocks in freelist [%d]!", i);
	 }
 }
}

Test(ye_memsuite, realloc_larger_block, .init = setup, .fini = teardown) {
 void *x = ye_malloc(sizeof(int));
 /* void *y = */ ye_malloc(10);
 x = ye_realloc(x, sizeof(int) * 10);

 freelist *fl = &seglist[find_list_index_from_size(32)];

 cr_assert_not_null(x, "x is NULL!");
 cr_assert_not_null(fl->head, "No block in expected free list!");
 cr_assert(fl->head->size << 4 == 32, "Free Block size not what was expected!");

 ye_header *header = (ye_header*)((char*)x - 8);
 cr_assert(header->size << 4 == 64, "Realloc'ed block size not what was expected!");
 cr_assert(header->alloc == 1, "Allocated bit is not set!");
}

Test(ye_memsuite, realloc_smaller_block_splinter, .init = setup, .fini = teardown) {
 void *x = ye_malloc(sizeof(int) * 8);
 void *y = ye_realloc(x, sizeof(char));

 cr_assert_not_null(y, "y is NULL!");
 cr_assert(x == y, "Payload addresses are different!");

 ye_header *header = (ye_header*)((char*)y - 8);
 cr_assert(header->alloc == 1, "Allocated bit is not set!");
 cr_assert(header->size << 4 == 48, "Block size not what was expected!");

 freelist *fl = &seglist[find_list_index_from_size(4048)];

 // There should be only one free block of size 4048 in list 3
 cr_assert_not_null(fl->head, "No block in expected free list!");
 cr_assert(fl->head->alloc == 0, "Allocated bit is set!");
 cr_assert(fl->head->size << 4 == 4048, "Free block size not what was expected!");
}

Test(ye_memsuite, realloc_smaller_block_free_block, .init = setup, .fini = teardown) {
 void *x = ye_malloc(sizeof(double) * 8);
 void *y = ye_realloc(x, sizeof(int));

 cr_assert_not_null(y, "y is NULL!");

 ye_header *header = (ye_header*)((char*)y - 8);
 cr_assert(header->size << 4 == 32, "Realloc'ed block size not what was expected!");
 cr_assert(header->alloc == 1, "Allocated bit is not set!");


 // After realloc'ing x, we can return a block of size 48 to the freelist.
 // This block will coalesce with the block of size 4016.
 freelist *fl = &seglist[find_list_index_from_size(4064)];

 cr_assert_not_null(fl->head, "No block in expected free list!");
 cr_assert(fl->head->alloc == 0, "Allocated bit is set!");
 cr_assert(fl->head->size << 4 == 4064, "Free block size not what was expected!");
}

//############################################

// This test depends on the values of the list 1 - 4 mins and maxes.
Test(ye_memsuite, list_index, .init = setup, .fini = teardown) {
	cr_assert(seg_index(31) == 1,
		"List 1 max not placed in list 1");
	cr_assert(seg_index(32) == 2,
		"List 2 min not placed in list 2");
	cr_assert(seg_index(PAGE_SIZE << 1) == NUM_LISTS - 1, "Page size not in largest list.");
}

Test(ye_memsuite, required_block_size, .init = setup, .fini = teardown) {
 cr_assert(ROUND(2) == 32, "Required size less than 32");
 cr_assert(ROUND(48) == 64, "Required size of perfectly fitting block is wrong");
 cr_assert(ROUND(47) == 64, "Required size of odd-sized block not padded");
 cr_assert(ROUND(1600) == 1616, "Required size of large number given incorrectly");
}

Test(ye_memsuite, nextblock_prevblock, .init = setup, .fini = teardown) {
 void *x = ye_malloc(64);
 void *y = ye_malloc(96);
 ye_header *xhdr = (ye_header *)(x - 8);
 ye_header *yhdr = (ye_header *)(y - 8);

 cr_assert_not_null(x, "x is NULL!");
 cr_assert_not_null(xhdr, "xhdr is NULL!");
 cr_assert_not_null(y, "y is NULL!");
 cr_assert_not_null(yhdr, "yhdr is NULL!");

 cr_assert(nextblock(xhdr) == yhdr, "Next block of alloc'd block not correctly computed");
 cr_assert(prevblock(yhdr) == xhdr, "Previous block of alloc'd block not correctly computed");
 ye_free(x);
 cr_assert(nextblock(xhdr) == yhdr, "Next block of free block not correctly computed");
 x = ye_malloc(64);
 ye_free(y);
 cr_assert(prevblock(yhdr) == xhdr, "Previous block of free block not correctly computed");
}

Test(ye_memsuite, malloc_overlap_pages, .init = setup, .fini = teardown) {
 void *x = ye_malloc(sizeof(int));
 void *y = ye_malloc(PAGE_SIZE);

 cr_assert_not_null(x, "x is NULL!");
 cr_assert_not_null(y, "y is NULL!");

 cr_assert(errno == 0, "Unexpected error given");
 ye_header *header = (ye_header *)((char *)y - 8);
 cr_assert(header->size << 4 == PAGE_SIZE + 16, "Block size was not what was expected!");
 cr_assert(header->alloc == 1, "Allocated bit is not set!");
//   cr_assert(header->padded == 0, "Padded bit is set!");

 cr_assert(heap_max() - heap_min() == 2*PAGE_SIZE, "Sbrk not called twice!");

}

Test(ye_memsuite, realloc_same_size_zero, .init = setup, .fini = teardown) {
 long *x = ye_malloc(sizeof(long) * 2);
 x[0] = 12345;
 x[1] = 67890;

 x = ye_realloc(x, sizeof(long) * 2);

 cr_assert_not_null(x, "x is NULL!");
 cr_assert(x[0] == 12345 && x[1] == 67890, "Realloc didn't preserve the payload!");

 x = ye_realloc(x, 0);

 cr_assert_null(x, "Realloc to zero didn't return NULL pointer!");
}
