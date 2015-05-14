// vim:ts=4:sw=4:expandtab
#include "all.h"
#include <getopt.h>

xcb_connection_t *connection;
xcb_window_t root;
Config config;

static void parse_args(int argc, char *argv[]) {
    int c;
    while ((c = getopt(argc, argv, "m:o:")) != -1) {
        switch (c) {
            case 'm':
                if (strcasecmp(optarg, "closest") == 0)
                    config.warp_mode = WM_CLOSEST;
                else if (strcasecmp(optarg, "relative") == 0)
                    config.warp_mode = WM_RELATIVE;
                else
                    bail("Unknown warp mode, bailing out.");

                break;
            case 'o':
                config.disable_randr = true;
                fake_outputs_create_outputs(optarg);
                fake_outputs_visualize();
                break;
            default:
                bail("Unknown command line arguments.");
        }
    }
}

static void on_xedgewarp_exit(void) {
    if (connection != NULL)
        xcb_disconnect(connection);
}

int main(int argc, char *argv[]) {
    atexit(on_xedgewarp_exit);

    initialize();
    parse_args(argc, argv);

    randr_query_outputs();
    event_initialize_tree();

    event_enter_loop();

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
