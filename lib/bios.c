/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: bios.c - Wrappers for BIOS functions
 */

#include <lib.h>

void
bios_putc(char c)
{
    regs_st regs;

    regs.h.ah = 0x0e;
    regs.h.al = c;
    regs.x.bx = 0;

    intr(0x10, &regs);
}

void
bios_puts(const char *s)
{
    while (*s) {
        if ((*s) == '\n') {
            bios_putc('\r');
        }

        bios_putc(*s++);
    }
}
