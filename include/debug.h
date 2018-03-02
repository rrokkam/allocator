#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include "segfreelist.h"  // for ye_header type definition

/* ANSI escape codes for colors */
#define NRM "\033[0m"
#define RED "\033[1;31m"
#define GRN "\033[1;32m"
#define YEL "\033[1;33m"
#define BLU "\033[1;34m"
#define MAG "\033[1;35m"

/* Debug printout macros with printf-style varargs */
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
 * Outputs the state of the heap to stderr.
 */
void ye_snapshotall();

/*
 * Outputs the state of the free lists to stderr.
 */
void ye_snapshotfree();

/*
 * Outputs the state of a block to stderr, given a pointer to the header
 * of the block. Calling this with a corrupted block may cause this
 * function to segfault.
 */
void ye_blockprint(ye_header *hdr);

/*
 * Outputs the state of a block to stderr, given a pointer to the payload
 * of the block. Calling this with a corrupted block may cause this
 * function to segfault.
 * 
 * This is equivalent to ye_blockprint(payload - HEADER_SIZE).
 */
void ye_varprint(void *payload);

#endif /* DEBUG_H */
