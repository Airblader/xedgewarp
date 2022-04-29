// vim:ts=4:sw=4:expandtab
#include "all.h"

/*
 * Closes the X connection and errors out with the given message.
 */
void bail(char *message) {
    if (connection != NULL)
        xcb_disconnect(connection);
    ELOG("Received error: %s", message);
    errx(EXIT_FAILURE, "%s", message);
}

/*
 * Checks the cookie for errors and returns the result.
 */
bool xcb_return_request_check(xcb_void_cookie_t cookie) {
    xcb_generic_error_t *error = xcb_request_check(connection, cookie);
    return error == NULL;
}

/*
 * Checks the cookie for errors and bails out if one was returned.
 */
void xcb_request_check_or_bail(xcb_void_cookie_t cookie, char *message) {
    if (!xcb_return_request_check(cookie))
        bail(message);
}
