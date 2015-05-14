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

/* Global configuration. */
extern Config config;

/*
 * We set this whenever a warp has occured so that we
 * prevent further warps until the pointer left the
 * edge at least once.
 */
extern bool has_warped;
