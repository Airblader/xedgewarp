// vim:ts=4:sw=4:expandtab

#include <stdlib.h>
#include <err.h>
#include <xcb/xcb.h>
#include "globals.h"

/*
 * Closes the X connection and errors out with the given message.
 */
void bail(char *message) {
    if (connection != NULL)
        xcb_disconnect(connection);
    errx(EXIT_FAILURE, message);
}
