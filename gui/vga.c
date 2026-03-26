/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: vga.c - Routines for programming the VGA
 */

#include <gui.h>

static const uint8_t gui_vga_dac_indexes[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
};

static void
gui_vga_set_color(int index, uint32_t rgb)
{
    uint8_t dac_index = index;

    dac_index = gui_vga_dac_indexes[index & 0x0F];

    outb(dac_index, 0x3C8);
    outb((rgb >> 18) & 0x3F, 0x3C9);
    outb((rgb >> 10) & 0x3F, 0x3C9);
    outb((rgb >>  2) & 0x3F, 0x3C9);
}

void
gui_vga_init(void)
{
    gui_vga_set_color(0x0f, 0x00ff00);
}
