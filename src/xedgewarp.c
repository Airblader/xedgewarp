// vim:ts=4:sw=4:expandtab
#include "all.h"

xcb_connection_t *connection;
xcb_window_t root;

int main(void) {
    initialize();

    randr_query_outputs();
    event_initialize_tree();

    event_enter_loop();

    xcb_disconnect(connection);
    exit(EXIT_SUCCESS);
}

/*
 * Initialize the X server connection.
 */
void initialize(void) {
    DLOG("Establishing a connection to the X server...");
    int display;
    connection = xcb_connect(NULL, &display);
    if (xcb_connection_has_error(connection)) {
        bail("Could not connect to X server, bailing out.");
    }

    xcb_screen_t *screen = xcb_aux_get_screen(connection, display);
    root = screen->root;
    DLOG("X server connection established. Let's rock!");
}
