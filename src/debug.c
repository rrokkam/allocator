#include "debug.h"
#include "allocator.h"

/*
 * Function which outputs the state of the free-lists to stderr.
 * Performs checks on the placement of the header and footer
 * See ye_snapshot section for details on output format.
 */
void ye_snapshot() {

}

/*
 * Function which prints human readable block format
 *
 * Note: If there are 'bad' addresses in your free list
 * this function will most likely segfault.
 *
 * @param block Address of the block header in memory.
 */
void ye_blockprint(void *block) {
//    fprintf(stderr,
}

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
void ye_varprint(void *data) {
    ye_blockprint(data - (YE_HEADER_SIZE >> 3));
}
