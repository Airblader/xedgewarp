// vim:ts=4:sw=4:expandtab
#pragma once

#include <xcb/xcb.h>
#include <xcb/randr.h>
#include "types.h"

/**
 * Query RandR outputs.
 *
 */
void randr_query_outputs(void);
