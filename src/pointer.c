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
static direction_t pointer_touches_any_border(position_t pointer) {
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
direction_t pointer_touches_border(position_t pointer) {
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
    position_t fake_position = pointer;
    direction_t direction;
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
void pointer_warp_to_adjacent_output(position_t pointer, direction_t direction) {
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
    position_t target = pointer_transform_position(pointer, current, output, direction);

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
 * Calculate the target position of the pointer such that it is only warped as little / far as necessary.
 * This means the pointer will be moved to the very edge of the target output.
 */
static position_t pointer_transform_position_closest(position_t pointer, Output *from, Output *to, direction_t direction) {
    position_t coordinates = {
        .x = 0,
        .y = 0
    };

    switch (direction) {
        case D_TOP:
            coordinates.y = to->rect.y + to->rect.height - 1;
            coordinates.x = to->rect.x;
            if (to->rect.x <= pointer.x)
                coordinates.x += to->rect.width - 1;

            break;
        case D_LEFT:
            coordinates.x = to->rect.x + to->rect.width - 1;
            coordinates.y = to->rect.y;
            if (to->rect.y <= pointer.y)
                coordinates.y += to->rect.height - 1;

            break;
        case D_BOTTOM:
            coordinates.y = to->rect.y;
            coordinates.x = to->rect.x;
            if (to->rect.x <= pointer.x)
                coordinates.x += to->rect.width - 1;

            break;
        case D_RIGHT:
            coordinates.x = to->rect.x;
            coordinates.y = to->rect.y;
            if (to->rect.y <= pointer.y)
                coordinates.y += to->rect.height - 1;

            break;
        default:
            ELOG("Unknown direction %d.", direction);
            break;
    }

    return coordinates;
}

/*
 * Calculate the target position of the pointer based on its relative position on the source output.
 * For example, if the pointer is warped to the right and was located 30% from the top of the source output,
 * it will be warped to the position 30% from the top of the target output.
 */
static position_t pointer_transform_position_relative(position_t pointer, Output *from, Output *to, direction_t direction) {
    /* To initially get to the correct output, we borrow the logic here and adapt afterwards. */
    position_t coordinates = pointer_transform_position_closest(pointer, from, to, direction);

    float percent;
    switch (direction) {
        case D_TOP:
        case D_BOTTOM:
            percent = (pointer.x - from->rect.x) / (float) from->rect.width;
            coordinates.x = to->rect.x + percent * to->rect.width;
            break;
        case D_LEFT:
        case D_RIGHT:
            percent = (pointer.y - from->rect.y) / (float) from->rect.height;
            coordinates.y = to->rect.y + percent * to->rect.height;
            break;
        default:
            ELOG("Unknown direction %d.", direction);
            break;
    }

    return coordinates;
}

/*
 * Takes the given position and transforms it to the position it should
 * have when warped from one output to the other.
 */
position_t pointer_transform_position(position_t pointer, Output *from, Output *to, direction_t direction) {
    switch (config.warp_mode) {
        case WM_CLOSEST:
            return pointer_transform_position_closest(pointer, from, to, direction);
        case WM_RELATIVE:
            return pointer_transform_position_relative(pointer, from, to, direction);
        default:
            bail("Unhandled warp mode, bailing out.");
            // never reached
            return (position_t) { .x = 0, .y = 0 };
    }
}
