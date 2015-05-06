// vim:ts=4:sw=4:expandtab

#include <xcb/xcb.h>
#include "event.h"
#include "globals.h"
#include "util.h"
#include "pointer.h"

/*
 * Register for events we need on the given window.
 */
void event_register_window(xcb_window_t window) {
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
void event_initialize_tree(void) {
    DLOG("Registering root window.");
    /* We grab the server for registering ourselves to the root window to make sure that
     * we will at least receive a CREATE_NOTIFY for any window that comes into existence
     * while we query the current tree. */
    xcb_grab_server(connection);
    event_register_window(root);
    xcb_ungrab_server(connection);
    xcb_flush(connection);

    DLOG("Querying and registering the current tree.");
    /* Query the tree and register ourselves onto every window that already exists. */
    xcb_query_tree_reply_t *reply = xcb_query_tree_reply(connection, xcb_query_tree(connection, root), NULL);
    if (reply == NULL)
        bail("Could not query the current tree, bailing out.");

    xcb_window_t *children = xcb_query_tree_children(reply);
    for (int i = 0; i < xcb_query_tree_children_length(reply); i++)
        event_register_window(children[i]);

    DLOG("Tree has been initialized.");
    FREE(reply);
}

/*
 * Enter the X event loop.
 */
void event_enter_loop(void) {
    xcb_generic_event_t *event;
    while ((event = xcb_wait_for_event(connection))) {
        switch (event->response_type & ~0x80) {
            case XCB_CREATE_NOTIFY:
                event_handle_create_notify((xcb_create_notify_event_t *) event);
                break;
            case XCB_MOTION_NOTIFY:
                event_handle_motion_notify((xcb_motion_notify_event_t *) event);
                break;
            default:
                break;
        }

        FREE(event);
    }
}

/*
 * Handle XCB_CREATE_NOTIFY.
 * This is used to register our events on the created window.
 */
void event_handle_create_notify(xcb_create_notify_event_t *event) {
    DLOG("Received CreateNotify event for window %d", event->window);
    event_register_window(event->window);
}

/*
 * Handle XCB_MOTION_NOTIFY.
 * This is used to detect when the mouse reaches the edge of an
 * output.
 */
void event_handle_motion_notify(xcb_motion_notify_event_t *event) {
    Position pointer = {
        .x = event->root_x,
        .y = event->root_y
    };

    /* check if we are touching the border of the current output */
    Direction direction = pointer_touches_border(pointer);
    if (direction == D_NONE)
        return;

    pointer_warp_to_adjacent_output(pointer, direction);
}
