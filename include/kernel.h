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

    KEY_1 = 0x02,
    KEY_2 = 0x03,
    KEY_3 = 0x04,
    KEY_4 = 0x05,
    KEY_5 = 0x06,
    KEY_6 = 0x07,
    KEY_7 = 0x08,
    KEY_8 = 0x09,
    KEY_9 = 0x0a,
    KEY_0 = 0x0b,

    KEY_A = 0x1e,
    KEY_B = 0x30,
    KEY_C = 0x2e,
    KEY_D = 0x20,
    KEY_E = 0x12,
    KEY_F = 0x21,
    KEY_G = 0x22,
    KEY_H = 0x23,
    KEY_I = 0x17,
    KEY_J = 0x24,
    KEY_K = 0x25,
    KEY_L = 0x26,
    KEY_M = 0x32,
    KEY_N = 0x31,
    KEY_O = 0x18,
    KEY_P = 0x19,
    KEY_Q = 0x10,
    KEY_R = 0x13,
    KEY_S = 0x1f,
    KEY_T = 0x14,
    KEY_U = 0x16,
    KEY_V = 0x2f,
    KEY_W = 0x11,
    KEY_X = 0x2d,
    KEY_Y = 0x15,
    KEY_Z = 0x2c,

    KEY_COMMA = 0x33,
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
