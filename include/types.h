// vim:ts=4:sw=4:expandtab
#pragma once

#include <sys/queue.h>
#include <xcb/xcb.h>
#include <xcb/randr.h>

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
