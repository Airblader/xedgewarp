// vim:ts=4:sw=4:expandtab
#include "all.h"
#include <X11/extensions/XInput2.h>

/* Forward declarations */
static void event_register_window_substructure_notify(xcb_window_t window);
static void event_register_window_motion(xcb_window_t window);

/*
 * Register for events we need on the given window.
 */
void event_register_window(xcb_window_t window) {
    event_register_window_substructure_notify(window);
    event_register_window_motion(window);
}

static void event_register_window_substructure_notify(xcb_window_t window) {
    DLOG("Setting event mask for window %d", window);
    const uint32_t mask = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;

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

/* TODO port this to XCB once xcb-xinput is stable. */
static void event_register_window_motion(xcb_window_t window) {
    XIEventMask masks[1];
    unsigned char mask[(XI_LASTEVENT + 7)/8];

    memset(mask, 0, sizeof(mask));
    XISetMask(mask, XI_RawMotion);

    masks[0].deviceid = XIAllMasterDevices;
    masks[0].mask_len = sizeof(mask);
    masks[0].mask = mask;

    XISelectEvents(display, window, masks, 1);
    XFlush(display);
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
        /* First, we see if this is a RawMotion event. Since Xinput2 uses
         * generic events, we need to cast the event and check its details
         * rather than using the extension offset. */
        xcb_ge_generic_event_t *generic_event = (xcb_ge_generic_event_t *) event;
        if (generic_event->response_type == XCB_GE_GENERIC &&
                generic_event->extension == xinput_ext_opcode &&
                generic_event->event_type == XCB_INPUT_RAW_MOTION) {

            event_handle_motion();
            continue;
        }

        /* From here on out we look at the event type itself. */
        int type = event->response_type & ~0x80;

        /* Check if this is a RandR event. */
        if (type == randr_ext_offset + XCB_RANDR_SCREEN_CHANGE_NOTIFY) {
            randr_query_outputs();
            continue;
        }

        switch (type) {
            case XCB_CREATE_NOTIFY:
                event_handle_create_notify((xcb_create_notify_event_t *) event);
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
 * This is called when we receive a RawMotion event.
 * It will query the pointer position and figure out whether
 * we need to warp the cursor etc.
 */
void event_handle_motion(void) {
    xcb_query_pointer_reply_t *reply = xcb_query_pointer_reply(connection,
            xcb_query_pointer(connection, root), NULL);
    if (reply == NULL) {
        ELOG("Failed to query the cursor position, ignoring this event.");
        return;
    }

    position_t pointer = (position_t) {
        .x = reply->root_x,
        .y = reply->root_y
    };

    FREE(reply);

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
