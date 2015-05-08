// vim:ts=4:sw=4:expandtab
#pragma once

#include <xcb/xcb.h>
#include <xcb/randr.h>
#include "types.h"

/**
 * Query RandR outputs.
 *
 */
void randr_query_outputs(void);

/**
 * Returns the output that contains this position.
 * Returns NULL if the position is not on any output.
 */
Output *randr_safely_get_output_containing(Position pointer);

/**
 * Returns the output that contains this position.
 * Never returns NULL.
 */
Output *randr_get_output_containing(Position pointer);

/**
 * Returns the output in the given direction to which the pointer
 * should be warped. Returns NULL if there is no output.
 */
Output *randr_next_output_in_direction(Position pointer, Direction direction);
