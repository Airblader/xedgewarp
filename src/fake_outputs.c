// vim:ts=4:sw=4:expandtab
#include "all.h"

/*
 * Set up fake RandR outputs from a string.
 */
void fake_outputs_create_outputs(char *outputs_str) {
    uint32_t x, y, width, height;
    const char *walk = outputs_str;
    while (sscanf(walk, "%ux%u+%u+%u", &width, &height, &x, &y) == 4) {
        Output *new = calloc(sizeof(Output), 1); 
        if (new == NULL)
            bail("Could not alloc space for fake output, bailing out.");

        new->id = xcb_generate_id(connection);

        new->rect.width = width;
        new->rect.height = height;
        new->rect.x = x;
        new->rect.y = y;

        TAILQ_INSERT_TAIL(&outputs, new, outputs);
        DLOG("Added fake output %d (x = %d / y = %d / w = %d / h = %d) to list of outputs.", new->id,
            new->rect.x, new->rect.y, new->rect.width, new->rect.height);

        char buf[1024];
        walk += sprintf(buf, "%ux%u+%u+%u", width, height, x, y) + 1;
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
    if (config.fake_outputs == NULL) {
        ELOG("Can only visualize fake outputs when they are set.");
        return;
    }

    xcb_screen_t *screen = xcb_aux_get_screen(connection, 0);

    Output *current;
    TAILQ_FOREACH(current, &outputs, outputs) {
        fake_outputs_open_window(&(current->rect), screen->black_pixel, screen->white_pixel);
    }
}
