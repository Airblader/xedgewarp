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

#define TLOG(message, ...)                                                             \
    do {                                                                               \
        if (config.log_level >= L_TRACE)                                               \
            printf("[%s:%d] TRACE: " message "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define DLOG(message, ...)                                                             \
    do {                                                                               \
        if (config.log_level >= L_DEBUG)                                               \
            printf("[%s:%d] DEBUG: " message "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ELOG(message, ...)                                                                      \
    do {                                                                                        \
        if (config.log_level >= L_ERROR)                                                        \
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
