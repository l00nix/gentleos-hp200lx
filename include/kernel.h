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
            uint8_t key_escaped;
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
