// vim:ts=4:sw=4:expandtab

#include <stdlib.h>
#include <err.h>
#include <xcb/xcb.h>
#include "util.h"
#include "globals.h"

/*
 * Closes the X connection and errors out with the given message.
 */
void bail(char *message) {
    if (connection != NULL)
        xcb_disconnect(connection);
    errx(EXIT_FAILURE, message);
}

/*
 * Checks the cookie for errors and bails out if one was returned.
 */
void xcb_request_check_or_bail(xcb_void_cookie_t cookie, char *message) {
    xcb_generic_error_t *error = xcb_request_check(connection, cookie);
    if (error != NULL)
        bail(message);
}
