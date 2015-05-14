// vim:ts=4:sw=4:expandtab
#include "all.h"

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
