/*
 * Copyright (c) 2014-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: lib.h - Basic standard library
 */

#ifndef _LIB_H_
#define _LIB_H_

#include <stdarg.h>

/* Silence IDE warnings */
#ifdef __clang__
#define far
#define interrupt
#endif

#ifdef __TURBOC__

typedef char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef long int32_t;
typedef unsigned long uint32_t;

#define INT32_MIN ((int32_t)0x80000000L)
#define INT32_MAX ((int32_t)0x7FFFFFFFL)

typedef long ssize_t;
typedef unsigned long size_t;

typedef unsigned long uintptr_t;

#pragma warn -rch
#pragma warn -par

#define MK_FP(seg, ofs) ((void far *) (((uint32_t)(seg) << 16) | (uint16_t)(ofs)))

typedef void interrupt far (*isr_handler_fn)(void);

#endif

#ifdef __WATCOMC__

#include <stdint.h>

typedef uint32_t size_t;
typedef int32_t ssize_t;

#define far __far
#define MK_FP(__s,__o) (((unsigned short)(__s)):>((void __near *)(__o)))

#endif

#include <config.h>

#define NULL ((void *)0)

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) < 0 ? -(a) : (a))

#define RETURN_IF_ALREADY_CALLED        \
    static int _already_called = 0;     \
    if (_already_called) {              \
        return;                         \
    }                                   \
    _already_called = 1;

/* lib/cpu.s */
typedef union {
    struct {
        uint16_t ax, bx, cx, dx;
        uint16_t bp, di, si, flags;
    } x;

    struct {
        uint8_t al, ah;
        uint8_t bl, bh;
        uint8_t cl, ch;
        uint8_t dl, dh;
    } h;
} regs_st;

uint16_t cpu_get_flags(void);
void cpu_set_flags(uint16_t flags);
void cpu_cli(void);
void cpu_hlt(void);
uint8_t inb(uint16_t port);
void outb(uint8_t value, uint16_t port);
void intr(int, regs_st *);

/* lib/string.c */
extern void far *memcpy_far(void far *dest, const void far *src, size_t n);
void far *memset_far(void far *dest, int c, size_t n);

#include "p_lib.h"

#endif /* _LIB_H_ */
