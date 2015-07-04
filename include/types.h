// vim:ts=4:sw=4:expandtab
#pragma once

#include <sys/queue.h>
#include <xcb/xcb.h>
#include <xcb/randr.h>

typedef struct position_t {
    uint32_t x;
    uint32_t y;
} position_t;

typedef struct Rect {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
} Rect;

/** Defines an output as detected by RandR. */
typedef struct Output {
    xcb_randr_output_t id;
    Rect rect;

    TAILQ_ENTRY(Output) outputs;
} Output;

TAILQ_HEAD(outputs_head, Output) outputs_head;

typedef enum direction_t {
    D_NONE = 0,
    D_TOP = 1 << 0,
    D_LEFT = 1 << 1,
    D_BOTTOM = 1 << 2,
    D_RIGHT = 1 << 3
} direction_t;

typedef enum warp_mode_t {
    /* Warp only as far as necessary. */
    WM_CLOSEST = 0,
    /* Warp relative to the position before warping. */
    WM_RELATIVE = 1
} warp_mode_t;

typedef enum cycle_mode_t {
    CM_NONE = 0,
    CM_VERTICAL = 1 << 0,
    CM_HORIZONTAL = 1 << 1
} cycle_mode_t;

typedef enum log_level_t {
    L_ERROR = 0,
    L_DEBUG = 1,
    L_TRACE = 2
} log_level_t;

typedef struct Config {
    /* If set, we use fake outputs and disable RandR (for testing purposes). */
    char *fake_outputs;

    /* Defines how the pointer should be warped. */
    warp_mode_t warp_mode;

    /* Cycle on the far outputs (as if the outputs form a torus shape). */
    cycle_mode_t cycle_mode;

    /* How much spam should we generate? */
    log_level_t log_level;

    /* If true, fork on startup */
    bool fork_mode;
} Config;
