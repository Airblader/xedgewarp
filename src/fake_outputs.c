// vim:ts=4:sw=4:expandtab
#include "all.h"

/*
 * Set up fake RandR outputs from a string.
 */
void fake_outputs_create_outputs(char *outputs_str) {
    /* We use some fake IDs starting at 990. We never use this ID anyway. */
    int id = 990;

    char *current;
    while ((current = strsep(&outputs_str, ","))) {
        Output *new = calloc(sizeof(Output), 1); 
        if (new == NULL) {
            ELOG("Could not alloc space for fake output %s, skipping it.", current);
            continue;
        } 

        new->id = id++;

        new->rect.width = atoi(strsep(&current, "x"));
        new->rect.height = atoi(strsep(&current, "+"));
        new->rect.x = atoi(strsep(&current, "+"));
        new->rect.y = atoi(strsep(&current, "+"));

        TAILQ_INSERT_TAIL(&outputs, new, outputs);
        DLOG("Added fake output %d (x = %d / y = %d / w = %d / h = %d) to list of outputs.", new->id,
            new->rect.x, new->rect.y, new->rect.width, new->rect.height);
    } 
}

/*
 * Opens a new window with the given rect for the geometry.
 * Sets override_redirect so that the window manager (if present) does not manage it.
 */
static void fake_outputs_open_window(Rect *rect, uint32_t background, uint32_t border) {
    xcb_window_t window = xcb_generate_id(connection);
    xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, root,
        rect->x + 1, rect->y + 1, rect->width - 2, rect->height - 2,
        1,
        XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_WINDOW_CLASS_COPY_FROM_PARENT,
        XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_OVERRIDE_REDIRECT,
        (uint32_t[]) { background, border, 1 });

    xcb_map_window(connection, window);
}

/*
 * Open marker windows for the fake outputs.
 */
void fake_outputs_visualize(void) {
    if (!config.disable_randr) {
        ELOG("Cannot visualize outputs when RandR is not disabled!");
        return;
    }

    xcb_screen_t *screen = xcb_aux_get_screen(connection, 0);

    Output *current;
    TAILQ_FOREACH(current, &outputs, outputs) {
        fake_outputs_open_window(&(current->rect), screen->black_pixel, screen->white_pixel);
    }
}
