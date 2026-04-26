/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: vga.c - Driver for CGA/VGA cards
 */

#include "lib.h"
#include <kernel.h>

global int krn_vga_theme_current = DEFAULT_VGA_THEME;

global const vga_theme_st krn_vga_themes[KRN_VGA_THEME_COUNT] = {
    { 0xcccccc, 0x000000, "gray / black" },
    { 0x00cc00, 0x000000, "green / black" },
};

static void
krn_vga_set_color(uint8_t idx, uint32_t rgb)
{
    uint8_t dac_index;
    regs_st regs;
	uint16_t *rgb_words = (uint16_t *)&rgb;
	uint16_t rgb_hi = rgb_words[1];
	uint16_t rgb_lo = rgb_words[0];
    uint8_t r6 = (uint8_t)((rgb_hi >> 2) & 0x3F);
    uint8_t g6 = (uint8_t)((rgb_lo >> 10) & 0x3F);
    uint8_t b6 = (uint8_t)((rgb_lo >> 2) & 0x3F);

    regs.h.ah = 0x10;
    regs.h.al = 0x07;
    regs.h.bl = idx;
    intr(0x10, &regs);
    dac_index = regs.h.bh;

    regs.h.ah = 0x10;
    regs.h.al = 0x10;
    regs.x.bx = dac_index;
    regs.h.dh = r6;
    regs.h.ch = g6;
    regs.h.cl = b6;

    intr(0x10, &regs);
}

global void
krn_vga_set_theme(int n)
{
    uint32_t fg, bg;

    if (n < 0 || n >= KRN_VGA_THEME_COUNT) {
        return;
    }

    krn_debug_printf("Setting color theme... ");

    krn_vga_set_color(0, krn_vga_themes[n].bg_color);
    krn_vga_set_color(3, krn_vga_themes[n].fg_color);
    krn_vga_theme_current = n;

    krn_debug_printf("ok\n");
}

global void
krn_vga_init(void)
{
    regs_st regs;

    krn_debug_printf("Initializing video... ");

    krn_debug_text_mode_enabled = 0;

    regs.h.ah = 0x00;
    regs.h.al = 0x04;
    intr(0x10, &regs);

    krn_debug_printf("ok\n");

    krn_vga_set_theme(krn_vga_theme_current);
}

global void
krn_vga_deinit(void)
{
    regs_st regs;

    regs.h.ah = 0x00;
    regs.h.al = 0x03;
    intr(0x10, &regs);

    krn_debug_text_mode_enabled = 1;
}
