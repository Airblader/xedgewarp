// vim:ts=4:sw=4:expandtab
#pragma once

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
