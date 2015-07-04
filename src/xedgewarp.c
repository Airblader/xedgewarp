// vim:ts=4:sw=4:expandtab
#include "all.h"
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef __VERSION
#define __VERSION "unknown"
#endif

typedef void (*callback)(void);

/* Forward declarations */
static void run();
static void safe_fork(callback child_callback);

Display *display;
xcb_connection_t *connection;
xcb_window_t root;

Config config = {
    .fake_outputs = NULL,
    .warp_mode = WM_CLOSEST,
    .cycle = false,
    .log_level = L_ERROR,
    .fork_mode = false
};

int main(int argc, char *argv[]) {
    atexit(on_xedgewarp_exit);
    parse_arguments(argc, argv);

    if (config.fork_mode) {
        safe_fork(run);
    } else {
        run();
    }
}

static void run() {
    initialize_x11();
    initialize_xedgewarp();

    extensions_init();
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
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        bail("Could not connect to X server, bailing out.");
    }

    connection = XGetXCBConnection(display);
    XSetEventQueueOwner(display, XCBOwnsEventQueue);

    xcb_screen_t *screen = xcb_aux_get_screen(connection, DefaultScreen(display));
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
    while ((c = getopt(argc, argv, "m:cl:o:bvh")) != -1) {
        switch (c) {
            case 'm':
                if (strcasecmp(optarg, "closest") == 0)
                    config.warp_mode = WM_CLOSEST;
                else if (strcasecmp(optarg, "relative") == 0)
                    config.warp_mode = WM_RELATIVE;
                else
                    bail("Unknown warp mode, bailing out.");

                break;
            case 'c':
                config.cycle = true;
                break;
            case 'l':
                if (strcasecmp(optarg, "error") == 0)
                    config.log_level = L_ERROR;
                else if (strcasecmp(optarg, "debug") == 0)
                    config.log_level = L_DEBUG;
                else if (strcasecmp(optarg, "trace") == 0)
                    config.log_level = L_TRACE;
                else
                    bail("Unknown log level, bailing out.");
                break;
            case 'o':
                config.fake_outputs = strdup(optarg);
                break;
            case 'b':
                config.fork_mode = true;
                break;
            case 'v':
                fprintf(stderr, "%s version %s\n", argv[0], __VERSION);
                exit(EXIT_SUCCESS);
                break;
            case 'h':
            default:
                fprintf(stderr, "Usage: %s [-b] [-m closest|relative] [-l error|debug|trace] [-v] [-h]", argv[0]);
                fprintf(stderr, "\n");
                fprintf(stderr, "\t-h display the usage and exit\n");
                fprintf(stderr, "\t-v display the version and exit\n");
                fprintf(stderr, "\t-b run in background mode, i.e. fork on startup\n");
                fprintf(stderr, "\t-m closest|relative\n"
                                "\t\tSpecifies how the mouse pointer should be warped.\n");
                fprintf(stderr, "\t-c\n"
                                "\t\tConnect the far edges of all outputs as if they were to form a torus shape.\n");
                fprintf(stderr, "\t-l error|debug|trace\n"
                                "\t\tSpecify the log level.\n");
                fprintf(stderr, "\n");
                exit(EXIT_FAILURE);
                break;
        }
    }
}

/* Double fork to prevent zombie processes */
static void safe_fork(callback child_callback) {
    pid_t pid = fork();
    if (!pid) {
        if (!fork()) {
            /* this is the child that keeps going */
            (*child_callback)();
        } else {
            /* the first child process exits */
            exit(EXIT_SUCCESS);
        }
    } else {
        /* this is the original process */
        /* wait for the first child to exit which it will immediately */
        waitpid(pid, NULL, 0);
    }
}
