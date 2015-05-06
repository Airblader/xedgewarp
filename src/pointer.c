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
    Output *output = randr_next_output_in_direction(pointer, direction);
    if (output == NULL) {
        DLOG("At position %d / %d, there is no more output in direction %d.",
            pointer.x, pointer.y, direction);
        return;
    }

    /* Determine where the pointer needs to be warped to. */
    Output *current = randr_get_output_containing(pointer);
    Position target = pointer_transform_position(pointer, current, output, direction);

    /* Let's do the pointer warp, again! */
    xcb_void_cookie_t cookie = xcb_warp_pointer_checked(connection, XCB_NONE, root, 0, 0, 0, 0,
        target.x, target.y);
    if (!xcb_return_request_check(cookie)) {
        ELOG("Failed to warp the pointer");
        return;
    }

    DLOG("Successfully warped pointer from %d / %d (%d) to %d / %d (%d)",
        pointer.x, pointer.y, current->id,
        target.x, target.y, output->id);
}

/*
 * Takes the given position and transforms it to the position it should
 * have when warped from one output to the other.
 */
Position pointer_transform_position(Position pointer, Output *from, Output *to, Direction direction) {
    // TODO implement this.
    return (Position) {
        .x = 0,
        .y = 0
    };
}
