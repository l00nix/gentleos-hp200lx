/*
 * Copyright (c) 2014-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: debug.c - Debug routines
 */

#include <kernel.h>

void
krn_debug_printf(const char *fmt, ...)
{
    int i, count;
    static char buf[4096];

    va_list args;

    va_start(args, fmt);
    count = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    for (i = 0; i < count; i++) {
        outb(buf[i], 0xe9);
    }
}

void
krn_debug_printf_bios(const char *fmt, ...)
{
    int i;
    static char buf[4096];

    va_list args;

    va_start(args, fmt);
    (void)vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    bios_puts(buf);
}

void
krn_debug_beep_adv(unsigned hz, unsigned msecs, unsigned count)
{
    unsigned i;

    for (i = 0; i < count; i++) {
        krn_speaker_play(hz);
        sleep(msecs);
        krn_speaker_stop();
        sleep(msecs);
    }
}

void
krn_debug_beep(void)
{
    krn_debug_beep_adv(300, 200, 1);
}
