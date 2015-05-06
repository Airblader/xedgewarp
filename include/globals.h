// vim:ts=4:sw=4:expandtab
#pragma once

#include <sys/queue.h>
#include <xcb/xcb.h>
#include "types.h"

/** Our gate to the other side. */
extern xcb_connection_t *connection;

/** The root window of this display. */
extern xcb_window_t root;

/** The list of RandR outputs. */
extern struct outputs_head outputs;
