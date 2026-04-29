/*
 * Copyright (c) 2014-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: kernel.h - Kernel API
 */

#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <lib.h>

typedef struct {
    uint16_t ofs;
    uint16_t seg;
} isr_st;

enum {
    KEY_UP = 0x48,
    KEY_DOWN = 0x50,
    KEY_LEFT = 0x4b,
    KEY_RIGHT = 0x4d,

    KEY_ENTER = 0x1c,
    KEY_SPACE = 0x39,
    KEY_ESC = 0x01,

    KEY_PGUP = 0x49,
    KEY_PGDN = 0x51,
    KEY_DEL = 0x53,

    KEY_LSHIFT = 0x2a,
    KEY_RSHIFT = 0x36,
    KEY_CTRL = 0x1d,
    KEY_ALT = 0x38,
};

enum {
    KEY_MOD_ESC     = 1 << 0,
    KEY_MOD_SHIFT   = 1 << 1,
    KEY_MOD_CTRL    = 1 << 2,
    KEY_MOD_ALT     = 1 << 3,
};

enum {
    EVENT_UNKNOWN = 0,
    EVENT_KEY_DOWN = 5,
    EVENT_KEY_UP = 6,
    EVENT_TIMER_TICK = 7,
};

typedef struct {
    uint8_t type;
    union {
        struct {
            uint8_t key_code;
            uint8_t key_char;
            uint8_t key_mods;
        } key;
        struct {
            uint32_t timer_msecs;
        } timer;
    } payload;
} event_st;

typedef struct {
    uint32_t fg_color;
    uint32_t bg_color;
    const char *name;
} vga_theme_st;

extern void *krn_link_start;
extern void *krn_link_end;
extern uint16_t krn_data_seg;

void far *krn_heap_alloc(uint16_t);

#include "p_kernel.h"

#endif /* _KERNEL_H_ */
