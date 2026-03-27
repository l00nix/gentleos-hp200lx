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
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} time_st;

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
        } key;
        struct {
            uint32_t timer_msecs;
        } timer;
    } payload;
} event_st;

extern void *krn_link_start;
extern void *krn_link_end;

#include "p_kernel.h"

#endif /* _KERNEL_H_ */
