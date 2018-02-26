#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include "segfreelist.h"  // for ye_header type definition

#define NRM "\033[0m"
#define RED "\033[1;31m"
#define GRN "\033[1;32m"
#define YEL "\033[1;33m"
#define BLU "\033[1;34m"
#define MAG "\033[1;35m"

#ifdef DEBUG
#define debug_print(type, fmt, ...)                                           \
    do {                                                                      \
        fprintf(stderr, type ": %s:%s:%d " NRM fmt "\n", __FILE__,            \
                __extension__ __FUNCTION__, __LINE__, ##__VA_ARGS__);         \
    } while (0)
#else
#define debug_print(type, fmt, ...)
#endif

#define debug(S, ...) debug_print(MAG "DEBUG", S, ##__VA_ARGS__)
#define info(S, ...) debug_print(BLU "INFO", S, ##__VA_ARGS__)
#define warn(S, ...) debug_print(YEL "WARN", S, ##__VA_ARGS__)
#define success(S, ...) debug_print(GRN "SUCCESS", S, ##__VA_ARGS__)
#define error(S, ...) debug_print(RED "ERROR", S, ##__VA_ARGS__)

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

#endif /* DEBUG_H */
