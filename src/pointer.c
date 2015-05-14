// vim:ts=4:sw=4:expandtab
#include "all.h"

/*
 * We set this whenever a warp has occured so that we
 * prevent further warps until the pointer left the
 * edge at least once.
 */
bool has_warped = false;

/*
 * Returns a bitmask of directions in which the pointer is touching
 * the output border. This function does not check whether the
 * border segment is "dead".
 */
static Direction pointer_touches_any_border(Position pointer) {
    Output *output = randr_get_output_containing(pointer);
    if (output == NULL)
        return D_NONE;

    Rect *rect = &(output->rect);
    int result = D_NONE;

    if (pointer.y == rect->y && pointer.x >= rect->x && pointer.x < rect->x + rect->width)
        result |= D_TOP;
    if (pointer.x == rect->x && pointer.y >= rect->y && pointer.y < rect->y + rect->height)
        result |= D_LEFT;
    if (pointer.y + 1 == rect->y + rect->height && pointer.x >= rect->x && pointer.x < rect->x + rect->width)
        result |= D_BOTTOM;
    if (pointer.x + 1 == rect->x + rect->width && pointer.y >= rect->y && pointer.y < rect->y + rect->height)
        result |= D_RIGHT;

    /* Remove D_NONE again if we found something. */
    if (result & D_TOP || result & D_LEFT || result & D_BOTTOM || result & D_RIGHT)
        result &= ~D_NONE;

    return result;
}

/*
 * Checks whether the given pointer is touching a "dead" border segment.
 * A border segment is considered dead when it is not directly neighboring
 * another output.
 */
Direction pointer_touches_border(Position pointer) {
    /* First, we check if the pointer is touching any border of the output it is on,
     * whether or not there is a neighboring output. */
    int directions = pointer_touches_any_border(pointer);

    /* Pointer is not on any border, so we can stop looking. */
    if (directions == D_NONE) {
        /* The pointer is not on any border segment, so we are eligible
         * for warping the next time it touches a dead border segment
         * again.
         * Note that we do this here for any border as otherwise, we will
         * incorrectly reset this after any warp. */
        has_warped = false;
        return D_NONE;
    }

    /* Otherwise, we need to check if the border segment is "dead", i.e., there is no
     * directly neighboring output as in such a case we don't need to do anything. */
    Position fake_position = pointer;
    Direction direction;
    if (directions & D_LEFT || directions & D_RIGHT) {
        direction = directions & D_LEFT ? D_LEFT : D_RIGHT;
        fake_position.x += direction == D_LEFT ? -1 : 1;
    } else if (directions & D_TOP || directions & D_BOTTOM) {
        direction = directions & D_TOP ? D_TOP : D_BOTTOM;
        fake_position.y += direction == D_TOP ? -1 : 1;
    } else
        bail("Congratulations, you found a bug. Please report it!");

    return randr_get_output_containing(fake_position) == NULL ? direction : D_NONE;
}

/*
 * Warps the pointer to the output in the given direction.
 */
void pointer_warp_to_adjacent_output(Position pointer, Direction direction) {
    Output *current = randr_get_output_containing(pointer);
    if (current == NULL) {
        ELOG("Cannot determine current output.");
        return;
    }

    Output *output = randr_next_output_in_direction(current, pointer, direction);
    if (output == NULL) {
        TLOG("At position %d / %d, there is no more output in direction %d.",
            pointer.x, pointer.y, direction);
        return;
    }

    /* Determine where the pointer needs to be warped to. */
    Position target = pointer_transform_position(pointer, current, output, direction);

    /* Let's do the pointer warp, again! */
    xcb_void_cookie_t cookie = xcb_warp_pointer_checked(connection, XCB_NONE, root, 0, 0, 0, 0,
        target.x, target.y);
    if (!xcb_return_request_check(cookie)) {
        ELOG("Failed to warp the pointer");
        return;
    }

    /* Store the fact that we warped to prevent further warping for now. */
    has_warped = true;

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

    if (direction == D_TOP) {
        coordinates.y = to->rect.y + to->rect.height - 1;
        coordinates.x = to->rect.x;
        if (to->rect.x <= pointer.x)
            coordinates.x += to->rect.width - 1;
    } else if (direction == D_LEFT) {
        coordinates.x = to->rect.x + to->rect.width - 1;
        coordinates.y = to->rect.y;
        if (to->rect.y <= pointer.y)
            coordinates.y += to->rect.height - 1;
    } else if (direction == D_BOTTOM) {
        coordinates.y = to->rect.y;
        coordinates.x = to->rect.x;
        if (to->rect.x <= pointer.x)
            coordinates.x += to->rect.width - 1;
    } else if (direction == D_RIGHT) {
        coordinates.x = to->rect.x;
        coordinates.y = to->rect.y;
        if (to->rect.y <= pointer.y)
            coordinates.y += to->rect.height - 1;
    }

    return coordinates;
}
