// vim:ts=4:sw=4:expandtab
#pragma once

#include <xcb/xcb.h>

/**
 * Enter the X event loop.
 */
void event_enter_loop(void);

/**
 * Handle XCB_CREATE_NOTIFY.
 * This is used to register our events on the created window.
 */
void event_handle_create_notify(xcb_create_notify_event_t *event);

/**
 * Handle XCB_MOTION_NOTIFY.
 * This is used to detect when the mouse reaches the edge of an
 * output.
 */
void event_handle_motion_notify(xcb_motion_notify_event_t *event);
