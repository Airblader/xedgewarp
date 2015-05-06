// vim:ts=4:sw=4:expandtab

#include <xcb/xcb.h>
#include "event.h"
#include "globals.h"
#include "util.h"

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
    // TODO
}

/*
 * Handle XCB_MOTION_NOTIFY.
 * This is used to detect when the mouse reaches the edge of an
 * output.
 */
void event_handle_motion_notify(xcb_motion_notify_event_t *event) {
    // TODO
}
