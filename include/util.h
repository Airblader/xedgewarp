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
 * Checks the cookie for errors and bails out if one was returned.
 */
void xcb_request_check_or_bail(xcb_void_cookie_t cookie, char *message);
