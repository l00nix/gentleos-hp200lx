/*
 * Copyright (c) 2014-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: timer.c - Driver for PIT 8254
 */

#include <kernel.h>

enum {
    PIT_CR0 = 0x40,
    PIT_CWR = 0x43,
    TIMER_HZ = 20,
};

#if !__CPROTO__
static isr_handler_fn saved_isr_handler;
#endif

volatile uint8_t krn_timer_is_cpu_idle = 0;

volatile static uint32_t timer_msecs = 0;
static uint32_t idle_ticks = 0;
static uint32_t total_ticks = 0;

static void interrupt
krn_timer_handle_intr()
{
    event_st event;

    timer_msecs += (1000 / TIMER_HZ);

    total_ticks++;
    if (krn_timer_is_cpu_idle) {
        idle_ticks++;
    }

    event.type = EVENT_TIMER_TICK;
    event.payload.timer.timer_msecs = timer_msecs;

    (void)krn_event_ipush(&event);
}

uint32_t
krn_timer_get_msecs(void)
{
    return timer_msecs;
}

uint8_t
krn_timer_get_cpu_usage(void)
{
    return 0;

    /*
    uint32_t idle, total;
    uint16_t flags = cpu_get_flags();

    cpu_cli();

    idle = idle_ticks;
    total = total_ticks;
    idle_ticks = 0;
    total_ticks = 0;

    cpu_set_flags(flags);

    if (total == 0) {
        return 0;
    }

    return (uint8_t)(100 - (idle * 100 / total));
    */
}

void
krn_timer_init(void)
{
    isr_handler_fn far *ivt = MK_FP(0, 0);
    uint16_t div = 1193180 / TIMER_HZ;

    /* Set Counter 0, write both LSB and MSB, use mode 3, binary counter */
    outb(0x36, PIT_CWR);

    /* Write LSB and MSB for counter 0 */
    outb((uint8_t)((div >> 0) & 0xFF), PIT_CR0);
    outb((uint8_t)((div >> 8) & 0xFF), PIT_CR0);

    saved_isr_handler = ivt[0x1c];
    ivt[0x1c] = krn_timer_handle_intr;
}

void
krn_timer_deinit(void)
{
    isr_handler_fn far *ivt = MK_FP(0, 0);
    ivt[0x1c] = saved_isr_handler;
}
