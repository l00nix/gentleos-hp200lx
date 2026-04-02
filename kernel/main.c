/*
 * Copyright (c) 2014-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: main.c - Kernel main function
 */

#include <kernel.h>
#include <gui.h>

extern uint16_t krn_data_seg;
extern uint32_t krn_marker_data_end;

#if ENABLE_TESTS
extern void tests_run(void);
#endif

void
krn_main(void)
{
    krn_debug_printf("\n");

    if (krn_marker_data_end != 0xf0cacc1a) {
        krn_debug_printf("Kernel loading failed\n");
        while (1);
    }

    krn_debug_printf("Kernel loaded successfully\n");

#if ENABLE_TESTS
    tests_run();
#endif

    krn_heap_init();
    krn_keyboard_init();
    krn_timer_init();
    rand_init();

    gui_main();

    while (1) {
        /* UNREACHABLE */
    };
}
