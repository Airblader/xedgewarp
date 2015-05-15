// vim:ts=4:sw=4:expandtab
#include "all.h"
#include <getopt.h>

#ifndef __VERSION
#define __VERSION "unknown"
#endif

xcb_connection_t *connection;
xcb_window_t root;
Config config;

int main(int argc, char *argv[]) {
    atexit(on_xedgewarp_exit);
    parse_arguments(argc, argv);

    initialize_x11();
    initialize_xedgewarp();

    randr_query_outputs();
    event_initialize_tree();

    event_enter_loop();

    exit(EXIT_SUCCESS);
}

/*
 * Initialize the X server connection.
 */
void initialize_x11(void) {
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

/*
 * Initialize xedgewarp specific things from the parsed
 * config.
 */
void initialize_xedgewarp(void) {
    if (config.fake_outputs != NULL) {
        fake_outputs_create_outputs(config.fake_outputs);
        fake_outputs_visualize();
    }
}

/*
 * Called when xedgewarp terminates.
 */
void on_xedgewarp_exit(void) {
    if (connection != NULL)
        xcb_disconnect(connection);

    FREE(config.fake_outputs);
}

/*
 * Parse command-line arguments.
 */
void parse_arguments(int argc, char *argv[]) {
    int c;
    while ((c = getopt(argc, argv, "m:o:vh")) != -1) {
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
                config.fake_outputs = strdup(optarg);
                break;
            case 'v':
                fprintf(stderr, "%s version %s\n", argv[0], __VERSION);
                exit(EXIT_SUCCESS);
                break;
            case 'h':
            default:
                fprintf(stderr, "Usage: %s [-m closest|relative]", argv[0]);
                fprintf(stderr, "\n");
                fprintf(stderr, "\t-h display the help / usage\n");
                fprintf(stderr, "\t-m closest|relative\n"
                                "\t\tSpecifies how the mouse pointer should be warped.\n");
                fprintf(stderr, "\n");
                exit(EXIT_FAILURE);
                break;
        }
    }
}
