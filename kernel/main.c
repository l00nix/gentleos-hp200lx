/*
 * Copyright (c) 2014-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: main.c - Kernel main function
 */

#include "lib.h"
#include <kernel.h>
#include <gui.h>

extern uint32_t krn_marker_data_end;

#ifndef __CPROTO__
isr_st far *krn_ivt;
#endif


#if ENABLE_TESTS
extern void tests_run(void);
#endif

void
krn_main(void)
{
    krn_debug_printf("\n");
    krn_ivt = MK_FP(0, 0);

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
    krn_vga_init();

    gui_main();

    while (1) {
        /* UNREACHABLE */
    };
}

void
krn_exit(void)
{
    regs_st regs;
    uint16_t *psp = 0;

    /* Do nothing if not under DOS */
    if (*psp != 0x20cd) {
        return;
    }

    krn_vga_deinit();
    krn_timer_deinit();
    krn_keyboard_deinit();

    intr(0x20, &regs);
}

void
krn_set_isr(uint8_t no, uint16_t seg, uint16_t ofs)
{
    uint16_t flags = cpu_get_flags();

    krn_ivt[no].seg = seg;
    krn_ivt[no].ofs = ofs;

    cpu_set_flags(flags);
}

void
krn_get_isr(uint8_t no, isr_st *dst)
{
    dst->seg = krn_ivt[no].seg;
    dst->ofs = krn_ivt[no].ofs;
}
