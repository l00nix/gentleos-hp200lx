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

static const unsigned char krn_keyboard_map_default[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static const unsigned char krn_keyboard_map_shift[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static isr_st saved_isr_handler;
extern void *krn_isr_keyboard;

global uint8_t
krn_keyboard_getc(void)
{
    event_st event;

    while (1) {
        krn_event_wait(&event);

        if (event.type == EVENT_KEY_DOWN) {
            return event.payload.key.key_char;
        }
    }
}

static void
krn_keyboard_handle_scancode(uint8_t scan)
{
    static uint8_t shift = 0;
    static uint8_t ctrl = 0;
    static uint8_t alt = 0;
    static int escaped = 0;

    event_st ev;
    int evtype;
    int is_key_down;

    if (scan == 0xe0) {
        escaped = 1;
        return;
    }

    ev.payload.key.key_escaped = escaped;
    escaped = 0;

    is_key_down = !(scan & 0x80);
    scan = scan & 0x7f;

    if (scan >= sizeof(krn_keyboard_map_default)) {
        return;
    }

    ev.type = is_key_down ? EVENT_KEY_DOWN : EVENT_KEY_UP;
    ev.payload.key.key_code = scan;
    ev.payload.key.key_char = shift ? krn_keyboard_map_shift[scan] : krn_keyboard_map_default[scan];

    if (ev.payload.key.key_code == KEY_LSHIFT || ev.payload.key.key_code == KEY_RSHIFT) {
        if (shift == is_key_down) {
            return;
        }
        shift = is_key_down;
    } else if (ev.payload.key.key_code == KEY_CTRL) {
        if (ctrl == is_key_down) {
            return;
        }
        ctrl = is_key_down;
    } else if (ev.payload.key.key_code == KEY_ALT) {
        if (alt == is_key_down) {
            return;
        }
        alt = is_key_down;
    }

#if DEBUG_KEYBOARD
    krn_debug_printf("Key %s: esc=%1X code=%02X char=%02X (%c)\n",
        ev.type == EVENT_KEY_UP ? "up" : "down",
        ev.payload.key.key_escaped,
        ev.payload.key.key_code,
        ev.payload.key.key_char,
        (ev.payload.key.key_char && ev.payload.key.key_char != '\n') ? ev.payload.key.key_char : ' '
    );
#endif

    if (ev.payload.key.key_code == KEY_DEL && ctrl && alt && is_key_down) {
        outb(0xFE, PS2_PORT_CMD);
        bios_reboot();
    }

    (void)krn_event_ipush(&ev);
}

global void
krn_keyboard_handle_intr(void)
{
    uint8_t ctrl;
    uint8_t scan = inb(PS2_PORT_DATA);

    krn_keyboard_handle_scancode(scan);

    ctrl = inb(0x61);
    outb(ctrl | 0x80, 0x61);
    outb(ctrl, 0x61);

    outb(0x20, 0x20);
}

global void
krn_keyboard_init(void)
{
    krn_debug_printf("Initializing keyboard... ");

    krn_get_isr(0x09, &saved_isr_handler);
    krn_set_isr(0x09, krn_data_seg, (uint16_t)(uint32_t)&krn_isr_keyboard);

    krn_debug_printf("ok\n");
}

global void
krn_keyboard_deinit(void)
{
    krn_set_isr(0x09, saved_isr_handler.seg, saved_isr_handler.ofs);
}
