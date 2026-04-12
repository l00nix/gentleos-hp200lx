/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: heap.c - Heap allocator
 */

#include <kernel.h>

static uint16_t krn_heap_current_seg;
static uint16_t krn_heap_current_ofs;

extern uint16_t krn_data_seg;

global void far *
krn_heap_alloc(uint16_t size)
{
    uint16_t seg, ofs;
    uint32_t remaining_space = 0x10000UL - krn_heap_current_ofs;
    void far *ret;

    if (size < 0xFFFF) {
        size = (size + 1) & ~1;
    }

    if (size == remaining_space) {
        seg = krn_heap_current_seg;
        ofs = krn_heap_current_ofs;
        krn_heap_current_seg += 0x1000;
        krn_heap_current_ofs = 0;
    } else if (size > remaining_space) {
        krn_heap_current_seg += 0x1000;
        seg = krn_heap_current_seg;
        ofs = 0;
        krn_heap_current_ofs = size;
    } else {
        seg = krn_heap_current_seg;
        ofs = krn_heap_current_ofs;
        krn_heap_current_ofs += size;
    }

    if (seg >= 0xa000) {
        krn_debug_printf("FATAL: Out of memory\n");
        krn_debug_beep_adv(300, 500, 3);
        while (1) { }
        /* UNREACHABLE */
    }

    ret = MK_FP(seg, ofs);

    memset_far(ret, 0, size);

    return ret;
}

global void
krn_heap_init(void)
{
    krn_heap_current_seg = krn_data_seg + 0x1000;
    krn_heap_current_seg = (krn_heap_current_seg + 0xfff) & 0xf000;
    krn_heap_current_ofs = 0;
}
