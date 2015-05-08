// vim:ts=4:sw=4:expandtab
#pragma once

#include "types.h"

/**
 * Checks whether the given pointer is touching a "dead" border segment.
 * A border segment is considered dead when it is not directly neighboring
 * another output.
 */
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
