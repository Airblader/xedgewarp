// vim:ts=4:sw=4:expandtab
#include "all.h"

static Direction pointer_touches_any_border(Position pointer) {
    Output *output = randr_get_output_containing(pointer);
    Rect *rect = &(output->rect);

    if (pointer.y == rect->y && pointer.x >= rect->x && pointer.x < rect->x + rect->width)
        return D_TOP;
    if (pointer.x == rect->x && pointer.y >= rect->y && pointer.y < rect->y + rect->height)
        return D_LEFT;
    if (pointer.y + 1 == rect->y + rect->height && pointer.x >= rect->x && pointer.x < rect->x + rect->width)
        return D_BOTTOM;
    if (pointer.x + 1 == rect->x + rect->width && pointer.y >= rect->y && pointer.y < rect->y + rect->height)
        return D_RIGHT;

    return D_NONE;
}

/*
 * Checks whether the given pointer is touching a "dead" border segment.
 * A border segment is considered dead when it is not directly neighboring
 * another output.
 */
Direction pointer_touches_border(Position pointer) {
    /* First, we check if the pointer is touching any border of the output it is on,
     * whether or not there is a neighboring output. */
    Direction direction = pointer_touches_any_border(pointer);

    /* Pointer is not on any border, so we can stop looking. */
    if (direction == D_NONE)
        return D_NONE;

    /* Otherwise, we need to check if the border segment is "dead", i.e., there is no
     * directly neighboring output as in such a case we don't need to do anything. */
    Position fake_position = pointer;
    if (direction == D_LEFT || direction == D_RIGHT)
        fake_position.x += direction == D_LEFT ? -1 : 1;
    else if (direction == D_TOP || direction == D_BOTTOM)
        fake_position.y += direction == D_TOP ? -1 : 1;
    else
        bail("Congratulations, you found a bug. Please report it!");

    return randr_safely_get_output_containing(fake_position) == NULL ? direction : D_NONE;
}

/*
 * Warps the pointer to the output in the given direction.
 */
void pointer_warp_to_adjacent_output(Position pointer, Direction direction) {
    Output *output = randr_next_output_in_direction(pointer, direction);
    if (output == NULL) {
        TLOG("At position %d / %d, there is no more output in direction %d.",
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
    Position coordinates = {
        .x = 0,
        .y = 0
    };

    // TODO simplify this
    // TODO avoid jumping back and forth in an endless loop
    if (direction == D_TOP) {
        coordinates.y = to->rect.y + to->rect.height - 1 - 1 /* TODO */;
        coordinates.x = to->rect.x;
        if (to->rect.x <= pointer.x)
            coordinates.x += to->rect.width - 1;
    } else if (direction == D_LEFT) {
        coordinates.x = to->rect.x + to->rect.width - 1 - 1 /* TODO */;
        coordinates.y = to->rect.y;
        if (to->rect.y <= pointer.y)
            coordinates.y += to->rect.height - 1;
    } else if (direction == D_BOTTOM) {
        coordinates.y = to->rect.y + 1 /* TODO */;
        coordinates.x = to->rect.x;
        if (to->rect.x <= pointer.x)
            coordinates.x += to->rect.width - 1;
    } else if (direction == D_RIGHT) {
        coordinates.x = to->rect.x + 1 /* TODO */;
        coordinates.y = to->rect.y;
        if (to->rect.y <= pointer.y)
            coordinates.y += to->rect.height - 1;
    }

    return coordinates;
}
