#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

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

#endif /* DEBUG_H */
