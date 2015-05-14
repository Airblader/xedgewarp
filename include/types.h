// vim:ts=4:sw=4:expandtab
#pragma once

#include <sys/queue.h>
#include <xcb/xcb.h>
#include <xcb/randr.h>

typedef struct Position {
    uint32_t x;
    uint32_t y;
} Position;

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

typedef enum Direction {
    D_NONE = 0,
    D_TOP = 1 << 0,
    D_LEFT = 1 << 1,
    D_BOTTOM = 1 << 2,
    D_RIGHT = 1 << 3
} Direction;

typedef enum warp_mode_t {
    /* Warp only as far as necessary. */
    WM_CLOSEST = 0,
    /* Warp relative to the position before warping. */
    WM_RELATIVE = 1
} warp_mode_t;

typedef struct Config {
    /* Disables RandR, which is useful for testing purposes. */
    bool disable_randr;

    /* Defines how the pointer should be warped. */
    warp_mode_t warp_mode;
} Config;
