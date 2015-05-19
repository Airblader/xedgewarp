// vim:ts=4:sw=4:expandtab
#pragma once

#include <xcb/xcb.h>
#include <xcb/randr.h>
#include "types.h"

/**
 * Query the presence of the RandR extension and
 * subscribe to events.
 */
void randr_init(void);

/**
 * Query RandR outputs.
 *
 */
void randr_query_outputs(void);

/**
 * Returns the output that contains this position.
 * Returns NULL if the position is not on any output.
 */
Output *randr_get_output_containing(position_t pointer);

/**
 * Returns the next output in the given direction relative to the specified
 * output. Returns NULL if no such output exists.
 * The given pointer must lie within the given output.
 */
Output *randr_next_output_in_direction(Output *from, position_t pointer, direction_t direction);
