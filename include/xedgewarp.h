// vim:ts=4:sw=4:expandtab
#pragma once

#include <xcb/xcb.h>

/**
 * Initialize the X server connection.
 */
void initialize(void);

/**
 * Register for the events we need on the given window.
 */
void window_register_events(xcb_window_t window);
