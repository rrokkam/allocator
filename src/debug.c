#include "debug.h"

/*
 * Function which outputs the state of the free-lists to stderr.
 * Performs checks on the placement of the header and footer
 * See sf_snapshot section for details on output format.
 */
void sf_snapshot() {

}

/*
 * Function which prints human readable block format
 *
 * Note: If there are 'bad' addresses in your free list
 * this function will most likely segfault.
 *
 * @param block Address of the block header in memory.
 */
void sf_blockprint(void* block) {
//    fprintf(stderr,
}

/*
 * Prints human readable block format from the address of the payload.
 * IE. subtracts header size from the data pointer to obtain the address
 * of the block header. Calls sf_blockprint internally to print.
 *
 * Note: If there are 'bad' addresses in your free list
 * this function will most likely segfault.
 *
 * @param data Pointer to payload data in memory
 * (value returned by sf_malloc).
 */
void sf_varprint(void *data) {

}
