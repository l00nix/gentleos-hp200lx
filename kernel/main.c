/*
 * Copyright (c) 2014-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: main.c - Kernel main function
 */

#include <kernel.h>
#include <gui.h>

extern uint16_t krn_data_seg;

void
krn_main(void)
{
    krn_heap_init();
    krn_keyboard_init();
    krn_timer_init();
    rand_init();

    gui_main();

    while (1) {
        /* UNREACHABLE */
    };
}
