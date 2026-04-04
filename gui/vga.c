/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: vga.c - Routines for programming the VGA
 */

#include <gui.h>

void
gui_vga_init(void)
{
    uint8_t dac_index;
    regs_st regs;

    krn_debug_text_mode_enabled = 0;

    regs.h.ah = 0x00;
    regs.h.al = 0x04;
    intr(0x10, &regs);

#if GUI_COLOR_OVERRIDE
    regs.h.ah = 0x10;
    regs.h.al = 0x07;
    regs.h.bl = 0x03;
    intr(0x10, &regs);
    dac_index = regs.h.bh;

    regs.h.ah = 0x10;
    regs.h.al = 0x10;
    regs.x.bx = dac_index;
    regs.h.dh = (GUI_COLOR_OVERRIDE >> 18) & 0x3F;
    regs.h.ch = (GUI_COLOR_OVERRIDE >> 10) & 0x3F;
    regs.h.cl = (GUI_COLOR_OVERRIDE >> 2) & 0x3F;
    intr(0x10, &regs);
#endif
}

void
gui_vga_deinit(void)
{
    regs_st regs;

    regs.h.ah = 0x00;
    regs.h.al = 0x03;
    intr(0x10, &regs);

    krn_debug_text_mode_enabled = 1;
}
