// vim:ts=4:sw=4:expandtab

#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include "xedgewarp.h"
#include "randr.h"
#include "event.h"
#include "types.h"
#include "globals.h"
#include "util.h"

xcb_connection_t *connection;
xcb_window_t root;

int main(void) {
    initialize();

    randr_query_outputs();
    window_initialize_tree();

    event_enter_loop();

    xcb_disconnect(connection);
    exit(EXIT_SUCCESS);
}

/*
 * Initialize the X server connection.
 */
void initialize(void) {
    DLOG("Establishing a connection to the X server...");
    int display;
    connection = xcb_connect(NULL, &display);
    if (xcb_connection_has_error(connection)) {
        bail("Could not connect to X server, bailing out.");
    }

    xcb_screen_t *screen = xcb_aux_get_screen(connection, display);
    root = screen->root;
    DLOG("X server connection established. Let's rock!");
}

/*
 * Register for events we need on the given window.
 */
void window_register_events(xcb_window_t window) {
    DLOG("Setting event mask for window %d", window);
    const uint32_t mask = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_POINTER_MOTION;

    xcb_void_cookie_t cookie = xcb_change_window_attributes_checked(connection,
        window, XCB_CW_EVENT_MASK, (uint32_t[]) { mask });
    xcb_request_check_or_bail(cookie, "Could not register for events on window, bailing out.");
}

/*
 * Register the root window and all its current children.
 * This function temporarily grabs the server.
 */
void window_initialize_tree(void) {
    DLOG("Registering root window.");
    /* We grab the server for registering ourselves to the root window to make sure that
     * we will at least receive a CREATE_NOTIFY for any window that comes into existence
     * while we query the current tree. */
    xcb_grab_server(connection);
    window_register_events(root);
    xcb_ungrab_server(connection);
    xcb_flush(connection);

    DLOG("Querying and registering the current tree.");
    /* Query the tree and register ourselves onto every window that already exists. */
    xcb_query_tree_reply_t *reply = xcb_query_tree_reply(connection, xcb_query_tree(connection, root), NULL);
    if (reply == NULL)
        bail("Could not query the current tree, bailing out.");

    xcb_window_t *children = xcb_query_tree_children(reply);
    for (int i = 0; i < xcb_query_tree_children_length(reply); i++)
        window_register_events(children[i]);

    DLOG("Tree has been initialized.");
    FREE(reply);
}
