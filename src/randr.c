// vim:ts=4:sw=4:expandtab
#include "all.h"

/* List of RandR outputs. */
struct outputs_head outputs = TAILQ_HEAD_INITIALIZER(outputs);

/*
 * Called by randr_query_outputs to insert / update a particular output in our list.
 *
 */
static void randr_handle_output(xcb_randr_output_t id, xcb_randr_get_output_info_reply_t *output,
        xcb_timestamp_t time) {
    DLOG("Handling output %d", id);
    if (output->crtc == XCB_NONE || output->connection == XCB_RANDR_CONNECTION_DISCONNECTED) {
        ELOG("Output %d seems to be disabled / disconnected, skipping it.", id);
        return;
    }

    xcb_randr_get_crtc_info_reply_t *crtc = xcb_randr_get_crtc_info_reply(connection,
        xcb_randr_get_crtc_info(connection, output->crtc, time), NULL);
    if (crtc == NULL) {
        ELOG("Could not receive CRTC information for output %d, skipping it.", id);
        return;
    }

    Output *new = calloc(sizeof(Output), 1);
    if (new == NULL) {
        ELOG("Could not alloc space for output %d, skipping it.", id);
        return;
    }

    new->id = id;
    new->rect = (Rect) {
        .x = crtc->x,
        .y = crtc->y,
        .width = crtc->width,
        .height = crtc->height
    };
    TAILQ_INSERT_TAIL(&outputs, new, outputs);

    DLOG("Added output %d to list of outputs.", id);
    FREE(crtc);
}

/**
 * Query RandR outputs.
 *
 */
// TODO Make this work for updating the list, not just initializing it.
void randr_query_outputs(void) {
    if (config.disable_randr) {
        DLOG("Skipping querying RandR outputs as RandR is disabled.");
        return;
    }

    DLOG("Querying RandR outputs...");
    xcb_randr_get_screen_resources_current_reply_t *reply = xcb_randr_get_screen_resources_current_reply(
        connection, xcb_randr_get_screen_resources_current(connection, root), NULL );
    if (reply == NULL)
        bail("Could not receive RandR outputs, bailing out.");

    /* This allows us to ensure that we get consistent information from the server. */
    xcb_timestamp_t time = reply->config_timestamp;

    int len = xcb_randr_get_screen_resources_current_outputs_length(reply);
    xcb_randr_output_t *randr_outputs = xcb_randr_get_screen_resources_current_outputs(reply);
    for (int i = 0; i < len; i++) {
        xcb_randr_get_output_info_reply_t *output = xcb_randr_get_output_info_reply(
            connection, xcb_randr_get_output_info(connection, randr_outputs[i], time), NULL);
        if (output == NULL) {
            DLOG("No output found for id = %d, skipping it.", randr_outputs[i]);
            continue;
        }

        randr_handle_output(randr_outputs[i], output, time);
        FREE(output);
    }

    FREE(reply);
}

/*
 * Returns the output that contains this position.
 * Returns NULL if the position is not on any output.
 */
Output *randr_get_output_containing(Position pointer) {
    Output *current;
    TAILQ_FOREACH(current, &outputs, outputs) {
        if (pointer.x >= current->rect.x && pointer.x < current->rect.x + current->rect.width &&
            pointer.y >= current->rect.y && pointer.y < current->rect.y + current->rect.height) {

            TLOG("Found output %d containing position %d / %d", current->id,
                pointer.x, pointer.y);
            return current;
        }
    }

    return NULL;
}

/*
 * Returns true if and only if
 *  - the second output lies (fully) in the given direction relative to the first output and
 *  - both outputs are touching (no gap in between).
 */
static bool randr_is_next_in_direction(Output *first_output, Output *second_output, Direction direction) {
    if (first_output == NULL || second_output == NULL) {
        ELOG("One of outputs %p / %p is NULL, stopping here.", first_output, second_output);
        return false;
    }

    Rect *first = &(first_output->rect);
    Rect *second = &(second_output->rect);

    switch (direction) {
        case D_TOP:
            return second->y + second->height == first->y;
        case D_LEFT:
            return second->x + second->width == first->x;
        case D_BOTTOM:
            return randr_is_next_in_direction(second_output, first_output, D_TOP);
        case D_RIGHT:
            return randr_is_next_in_direction(second_output, first_output, D_LEFT);
        default:
            ELOG("Received unknown value %d and don't know what to do with it.", direction);
            return false;
    }
}

/*
 * Returns the next output in the given direction relative to the specified
 * output. Returns NULL if no such output exists.
 */
Output *randr_next_output_in_direction(Output *from, Direction direction) {
    Output *output;
    TAILQ_FOREACH(output, &outputs, outputs) {
        if (randr_is_next_in_direction(from, output, direction))
            return output;
    }

    return NULL;
}
