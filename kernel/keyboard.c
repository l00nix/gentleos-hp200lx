/*
 * Copyright (c) 2014-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: keyboard.c - Driver for PS/2 keyboard
 */

#include <kernel.h>

enum {
    PS2_PORT_DATA = 0x60,
    PS2_PORT_CMD  = 0x64,
};

enum {
    KBD_DEBUG = 0,
};

static const unsigned char krn_keyboard_map_default[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0,
    '-', 252, 0, 253, '+', 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static const unsigned char krn_keyboard_map_shift[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0,
    '-', 252, 0, 253, '+', 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static void
krn_keyboard_finish_handling()
{
    uint8_t p61 = inb(0x61);
    outb(p61 | 0x80, 0x61);
    outb(p61, 0x61);

    outb(0x20, 0x20);
}

static void interrupt
krn_keyboard_handle_intr()
{
    static uint8_t shift = 0;
    static uint8_t ctrl = 0;
    static uint8_t alt = 0;

    uint8_t scan;
    event_st ev;
    int evtype;

    scan = inb(PS2_PORT_DATA);

    if (scan == 0xe0) {
        krn_keyboard_finish_handling();
        return;
    }

    evtype = scan & 0x80 ? EVENT_KEY_UP : EVENT_KEY_DOWN,
    scan = scan & 0x7f;

    if (scan >= sizeof(krn_keyboard_map_default)) {
        krn_keyboard_finish_handling();
        return;
    }

    ev.type = evtype;
    ev.payload.key.key_code = scan;
    ev.payload.key.key_char = shift ? krn_keyboard_map_shift[scan] : krn_keyboard_map_default[scan];

    if (KBD_DEBUG) {
        krn_debug_printf("keyboard event: %s code=%02X char=%02X (%c)\n",
            ev.type == EVENT_KEY_UP ? "up" : "down",
            ev.payload.key.key_code,
            ev.payload.key.key_char,
            ev.payload.key.key_char ? ev.payload.key.key_char : ' '
        );
    }

    if (ev.payload.key.key_code == 0x2a || ev.payload.key.key_code == 0x36) {
        shift = (ev.type == EVENT_KEY_UP) ? 0 : 1;
    } else if (ev.payload.key.key_code == 0x1d) {
        ctrl = (ev.type == EVENT_KEY_UP) ? 0 : 1;
    } else if (ev.payload.key.key_code == 0x38) {
        alt = (ev.type == EVENT_KEY_UP) ? 0 : 1;
    } else if (ev.payload.key.key_code == 0x53 && ctrl && alt && ev.type == EVENT_KEY_DOWN) {
        outb(0xFE, PS2_PORT_CMD);
    } else {
        (void)krn_event_ipush(&ev);
    }

    krn_keyboard_finish_handling();
}

void
krn_keyboard_init(void)
{
    isr_handler_fn far *ivt = MK_FP(0, 0);
    ivt[0x09] = krn_keyboard_handle_intr;
}
