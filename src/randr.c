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
 * Returns the output in the given direction to which the pointer
 * should be warped. Returns NULL if there is no output.
 */
Output *randr_next_output_in_direction(Position pointer, Direction direction) {
    Output *output = NULL;

    Output *current;
    // TODO ensure that the output is actually touching the current one
    TAILQ_FOREACH(current, &outputs, outputs) {
        Rect *rect = &(current->rect);

        if ((direction == D_LEFT && rect->x + rect->width <= pointer.x) ||
            (direction == D_RIGHT && rect->x > pointer.x)) {

            output = current;
            break;
        }

        if ((direction == D_TOP && rect->y + rect->height <= pointer.y) ||
            (direction == D_BOTTOM && rect->y > pointer.y)) {

            output = current;
            break;
        }
    }

    TLOG("Found output %d in direction %d.", output == NULL ? -1 : output->id, direction);
    return output;
}
