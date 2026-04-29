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

static isr_st saved_isr_handler;
extern void *krn_isr_keyboard;

global uint8_t
krn_keyboard_getc(void)
{
    event_st event;

    while (1) {
        krn_event_wait(&event);

        if (event.type == EVENT_KEY_DOWN) {
            return event.payload.key.key_code;
        }
    }
}

static void
krn_keyboard_handle_scancode(uint8_t scancode)
{
    static uint8_t lshift = 0;
    static uint8_t rshift = 0;
    static uint8_t ctrl = 0;
    static uint8_t alt = 0;
    static int last_scan_was_e0 = 0;

    event_st ev;
    uint8_t key_code = scancode & 0x7f;
    int is_key_down = !(scancode & 0x80);
    int is_key_escaped = last_scan_was_e0;
    uint8_t *mod;

    if (scancode == 0xe0) {
        last_scan_was_e0 = 1;
        return;
    }

    last_scan_was_e0 = 0;

    switch (key_code) {
    case KEY_LSHIFT: mod = &lshift; break;
    case KEY_RSHIFT: mod = &rshift; break;
    case KEY_CTRL: mod = &ctrl; break;
    case KEY_ALT: mod = &alt; break;
    default: mod = 0;
    }

    /* Ignore duplicate key presses of modifiers */
    if (mod && *mod == is_key_down) {
        return;
    }

    if (mod) {
        *mod = is_key_down;
    }

    ev.type = is_key_down ? EVENT_KEY_DOWN : EVENT_KEY_UP;
    ev.payload.key.key_code = key_code;
    ev.payload.key.key_mods =
        (KEY_MOD_ESC * is_key_escaped) |
        (KEY_MOD_SHIFT * lshift) |
        (KEY_MOD_SHIFT * rshift) |
        (KEY_MOD_CTRL * ctrl) |
        (KEY_MOD_ALT * alt);

#if DEBUG_KEYBOARD
    krn_debug_printf("Key %s: code=%02X mods=%02X\n",
        ev.type == EVENT_KEY_UP ? "up" : "down",
        ev.payload.key.key_mods,
        ev.payload.key.key_code,
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
