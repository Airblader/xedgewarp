// vim:ts=4:sw=4:expandtab
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>

#define FREE(p)          \
    do {                 \
        if (p != NULL) { \
            free(p);     \
            p = NULL;    \
        }                \
    } while (0)

#define ABS(x) ((x) < 0) ? -(x) : (x)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)

#if 0
#define TLOG(message, ...)        \
    DLOG(message, ##__VA_ARGS__);
#else
#define TLOG(message, ...) \
    do {} while (0)
#endif

#define DLOG(message, ...)                                                         \
    do {                                                                           \
        printf("[%s:%d] DEBUG: " message "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ELOG(message, ...)                                                                  \
    do {                                                                                    \
        fprintf(stderr, "[%s:%d] ERROR: " message "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

/**
 * Closes the X connection and errors out with the given message.
 */
void bail(char *message);

/**
 * Checks the cookie for errors and returns the result.
 */
bool xcb_return_request_check(xcb_void_cookie_t cookie);
