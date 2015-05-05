// vim:ts=4:sw=4:expandtab
#pragma once

#include <xcb/xcb.h>

#define FREE(p)          \
    do {                 \
        if (p != NULL) { \
            free(p);     \
            p = NULL;    \
        }                \
    } while (0)

/**
 * Closes the X connection and errors out with the given message.
 */
void bail(char *message);

/**
 * Checks the cookie for errors and bails out if one was returned.
 */
void xcb_request_check_or_bail(xcb_void_cookie_t cookie, char *message);
