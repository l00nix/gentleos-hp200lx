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

uint16_t
bios_getc(void)
{
    regs_st regs;

    regs.h.ah = 0x00;
    intr(0x16, &regs);

    return regs.x.ax;
}

void
bios_uart_putc(char c)
{
    regs_st regs;

    regs.h.ah = 0x01;
    regs.h.al = c;
    regs.x.dx = 0;

    intr(0x14, &regs);
}

void
bios_uart_puts(const char *s)
{
    while (*s) {
        if ((*s) == '\n') {
            bios_uart_putc('\r');
        }

        bios_uart_putc(*s++);
    }
}

void
bios_get_time(time_st *t)
{
    regs_st regs;

    regs.h.ah = 0x02;

    intr(0x1a, &regs);

    t->hour = ((regs.h.ch >> 4) & 0x0F) * 10 + (regs.h.ch & 0x0F);
    t->minute = ((regs.h.cl >> 4) & 0x0F) * 10 + (regs.h.cl & 0x0F);
    t->second = ((regs.h.dh >> 4) & 0x0F) * 10 + (regs.h.dh & 0x0F);
}

void
bios_get_date(date_st *d)
{
    regs_st regs;

    regs.h.ah = 0x04;

    intr(0x1a, &regs);

    d->year = ((regs.h.ch >> 4) & 0x0F) * 1000
        + (regs.h.ch & 0x0F) * 100
        + ((regs.h.cl >> 4) & 0x0F) * 10
        + (regs.h.cl & 0x0F);
    d->month = ((regs.h.dh >> 4) & 0x0F) * 10 + (regs.h.dh & 0x0F);
    d->day = ((regs.h.dl >> 4) & 0x0F) * 10 + (regs.h.dl & 0x0F);
}
