// vim:ts=4:sw=4:expandtab
#include "all.h"

/* The offset of the RandR extension. */
int randr_ext_offset = -1;

/* The major opcode of the Xinput2 extension. */
int xinput_ext_opcode = -1;

/* Forward declarations */
static void extensions_init_randr(void);
static void extensions_init_xinput(void);

/*
 * Initialize the required X11 extensions.
 */
void extensions_init(void) {
    extensions_init_randr();
    extensions_init_xinput();
}

static void extensions_init_randr(void) {
    const xcb_query_extension_reply_t *reply = xcb_get_extension_data(connection, &xcb_randr_id);
    if (!reply->present)
        bail("Your X server does not support the RandR extension, bailing out.");

    randr_ext_offset = reply->first_event;

    xcb_randr_select_input(connection, root,
        XCB_RANDR_NOTIFY_MASK_SCREEN_CHANGE |
        XCB_RANDR_NOTIFY_MASK_OUTPUT_CHANGE |
        XCB_RANDR_NOTIFY_MASK_CRTC_CHANGE |
        XCB_RANDR_NOTIFY_MASK_OUTPUT_PROPERTY);
    xcb_flush(connection);
}

static void extensions_init_xinput(void) {
    /* First we check if XInput is available at all. */
    int event, error;
    if (!XQueryExtension(display, "XInputExtension", &xinput_ext_opcode, &event, &error))
        bail("XInput extension is not available.");

    /* Now we check if we have the correct version. We need >= 2.2. */
    int major_opcode = 2;
    int minor_opcode = 2;

    int result = XIQueryVersion(display, &major_opcode, &minor_opcode);
    if (result == BadRequest)
        bail("XI2 is not supported or not in a sufficient version.");
    else if (result != Success)
        bail("Failed to query XI extension. This is a bug.");
}
