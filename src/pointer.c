// vim:ts=4:sw=4:expandtab
#include "all.h"

/*
 * Returns D_NONE if the pointer is not on the edge of
 * the current output. Otherwise, it returns on which
 * edge the pointer is touching.
 */
Direction pointer_touches_border(Position pointer) {
    Output *current;
    TAILQ_FOREACH(current, &outputs, outputs) {
        Rect rect = current->rect;

        if (pointer.y == rect.y && pointer.x >= rect.x && pointer.x < rect.x + rect.width)
            return D_TOP;
        if (pointer.x == rect.x && pointer.y >= rect.y && pointer.y < rect.y + rect.height)
            return D_LEFT;
        if (pointer.y + 1 == rect.y + rect.height && pointer.x >= rect.x && pointer.x < rect.x + rect.width)
            return D_BOTTOM;
        if (pointer.x + 1 == rect.x + rect.width && pointer.y >= rect.y && pointer.y < rect.y + rect.height)
            return D_RIGHT;
    }

    return D_NONE;
}

/*
 * Warps the pointer to the output in the given direction.
 */
void pointer_warp_to_adjacent_output(Position pointer, Direction direction) {
    // TODO implement this
}
