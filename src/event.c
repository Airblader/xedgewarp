// vim:ts=4:sw=4:expandtab

#include <xcb/xcb.h>
#include "event.h"
#include "globals.h"
#include "util.h"

/*
 * Enter the X event loop.
 */
void event_enter_loop(void) {
    xcb_generic_event_t *event;
    while ((event = xcb_wait_for_event(connection))) {
        // TODO handle event

        FREE(event);
    }
}
