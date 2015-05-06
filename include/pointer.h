// vim:ts=4:sw=4:expandtab
#pragma once

#include "types.h"

/**
 * Returns D_NONE if the pointer is not on the edge of
 * the current output. Otherwise, it returns on which
 * edge the pointer is touching.
 */
// TODO only return direction if it's a "dead" border
Direction pointer_touches_border(Position pointer);

/**
 * Warps the pointer to the output in the given direction.
 */
void pointer_warp_to_adjacent_output(Position pointer, Direction direction);

/**
 * Takes the given position and transforms it to the position it should
 * have when warped from one output to the other.
 */
Position pointer_transform_position(Position pointer, Output *from, Output *to, Direction direction);
