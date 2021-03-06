// vim:ts=4:sw=4:expandtab
#pragma once

#include <xcb/xcb.h>

/**
 * Register for the events we need on the given window.
 */
void event_register_window(xcb_window_t window);

/**
 * Register the root window and all its current children.
 * This function temporarily grabs the server.
 */
void event_initialize_tree(void);

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
 * This is called when we receive a RawMotion event.
 * It will query the pointer position and figure out whether
 * we need to warp the cursor etc.
 */
void event_handle_motion(void);
