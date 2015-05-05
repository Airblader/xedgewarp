// vim:ts=4:sw=4:expandtab

#include <stdlib.h>
#include <sys/queue.h>
#include <xcb/xcb.h>
#include <xcb/randr.h>
#include "randr.h"
#include "globals.h"
#include "types.h"
#include "util.h"

/* List of RandR outputs. */
struct outputs_head outputs = TAILQ_HEAD_INITIALIZER(outputs);

/*
 * Called by randr_query_outputs to insert / update a particular output in our list.
 *
 */
static void randr_handle_output(xcb_randr_output_t id, xcb_randr_get_output_info_reply_t *output,
        xcb_timestamp_t time) {
    if (output->crtc == XCB_NONE || output->connection == XCB_RANDR_CONNECTION_DISCONNECTED)
        return;

    xcb_randr_get_crtc_info_reply_t *crtc = xcb_randr_get_crtc_info_reply(connection,
        xcb_randr_get_crtc_info(connection, output->crtc, time), NULL);
    if (crtc == NULL)
        return;

    Output *new = calloc(sizeof(Output), 1);
    if (new == NULL)
        return;

    new->id = id;
    new->rect = (Rect) {
        .x = crtc->x,
        .y = crtc->y,
        .width = crtc->width,
        .height = crtc->height
    };
    TAILQ_INSERT_TAIL(&outputs, new, outputs);

    FREE(crtc);
}

/**
 * Query RandR outputs.
 *
 */
// TODO Make this work for updating the list, not just initializing it.
void randr_query_outputs(void) {
    xcb_randr_get_screen_resources_current_reply_t *reply = xcb_randr_get_screen_resources_current_reply(
        connection, xcb_randr_get_screen_resources_current(connection, root), NULL );
    if (reply == NULL)
        bail("could not receive RandR outputs, bailing out.");

    /* This allows us to ensure that we get consistent information from the server. */
    xcb_timestamp_t time = reply->config_timestamp;

    int len = xcb_randr_get_screen_resources_current_outputs_length(reply);
    xcb_randr_output_t *randr_outputs = xcb_randr_get_screen_resources_current_outputs(reply);
    for (int i = 0; i < len; i++) {
        xcb_randr_get_output_info_reply_t *output = xcb_randr_get_output_info_reply(
            connection, xcb_randr_get_output_info(connection, randr_outputs[i], time), NULL);
        if (output == NULL)
            continue;

        randr_handle_output(randr_outputs[i], output, time);
        FREE(output);
    }

    FREE(reply);
}
