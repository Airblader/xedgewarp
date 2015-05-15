// vim:ts=4:sw=4:expandtab
#pragma once

#include "types.h"

/**
 * Checks whether the given pointer is touching a "dead" border segment.
 * A border segment is considered dead when it is not directly neighboring
 * another output.
 */
direction_t pointer_touches_border(position_t pointer);

/**
 * Warps the pointer to the output in the given direction.
 */
void pointer_warp_to_adjacent_output(position_t pointer, direction_t direction);

/**
 * Takes the given position and transforms it to the position it should
 * have when warped from one output to the other.
 */
position_t pointer_transform_position(position_t pointer, Output *from, Output *to, direction_t direction);
