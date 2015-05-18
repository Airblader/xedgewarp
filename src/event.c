// vim:ts=4:sw=4:expandtab
#include "all.h"

/*
 * Register for events we need on the given window.
 */
void event_register_window(xcb_window_t window) {
    DLOG("Setting event mask for window %d", window);
    const uint32_t mask = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_POINTER_MOTION;

    xcb_void_cookie_t cookie = xcb_change_window_attributes_checked(connection,
        window, XCB_CW_EVENT_MASK, (uint32_t[]) { mask }); 
    xcb_generic_error_t *error = xcb_request_check(connection, cookie);
    if (error == NULL)
        return;

    /* The window might not exist anymore by now. In this case, we can safely ignore the error. */
    if (error->error_code == XCB_WINDOW)
        DLOG("Window does not exist anymore, cannot set event mask on it.");
    else
        ELOG("Received error %d when trying to register event mask.", error->error_code);

    FREE(error);
}

static void event_initialize_tree_on(xcb_window_t window) {
    /* Register the given window. */
    event_register_window(window);

    DLOG("Querying the tree for window %d", window);
    xcb_query_tree_reply_t *reply = xcb_query_tree_reply(connection, xcb_query_tree(connection, window), NULL);
    if (reply == NULL)
        bail("Could not query the tree, bailing out.");

    /* Register all children as well. */
    xcb_window_t *children = xcb_query_tree_children(reply);
    for (int i = 0; i < xcb_query_tree_children_length(reply); i++) {
        event_initialize_tree_on(children[i]);
    }

    FREE(reply);
}

/*
 * Register the root window and all its current children.
 * This function temporarily grabs the server.
 */
void event_initialize_tree(void) {
    /* We grab the server for this to avoid race conditions. */
    xcb_grab_server(connection);
    event_initialize_tree_on(root);
    xcb_ungrab_server(connection);

    /* Make sure we push all these requests to X as quickly as possible. */
    xcb_flush(connection);

    DLOG("Tree has been initialized.");
}

/*
 * Enter the X event loop.
 */
void event_enter_loop(void) {
    /* This output is important for tests. */
    DLOG("Entering event loop.");

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
    position_t pointer = {
        .x = event->root_x,
        .y = event->root_y
    };

    direction_t direction = pointer_touches_border(pointer);
    if (direction == D_NONE)
        return;

    /* Check if we already warped without the pointer having left dead border
     * segments in the meantime. If so, we ignore this. */
    if (has_warped) {
        DLOG("Pointer has already been warped, not warping it again.");
        return;
    }

    DLOG("Touching a dead border segment at %d / %d.", pointer.x, pointer.y);
    pointer_warp_to_adjacent_output(pointer, direction);
}
