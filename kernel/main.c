// --------------------------------------------------------------------------------------
// Copyright (c) 2014-2026 luke8086
// Distributed under the terms of GPL-2 License
// --------------------------------------------------------------------------------------
// File: main.c - Kernel main function
// --------------------------------------------------------------------------------------

#include <kernel.h>
#include <gui.h>

void
krn_main(void)
{
    krn_timer_init();
    krn_keyboard_init();

    rand_init();
    gui_main();

    while (1);
}
