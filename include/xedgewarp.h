// vim:ts=4:sw=4:expandtab
#pragma once

#include <xcb/xcb.h>

/**
 * Initialize the X server connection.
 */
void initialize_x11(void);

/**
 * Initialize xedgewarp specific things from the parsed
 * config.
 */
void initialize_xedgewarp(void);

/**
 * Called when xedgewarp terminates.
 */
void on_xedgewarp_exit(void);

/**
 * Parse command-line arguments.
 */
void parse_arguments(int argc, char *argv[]);
