/*
 * Copyright (c) 2014-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: uart.c - Driver for UART 8250
 */

#include <kernel.h>

enum {
    UART_BASE = 0x3F8,
    UART_RBR = 0, /* receiver buffer register */
    UART_THR = 0, /* transmitter holding register */
    UART_DLL = 0, /* divisor latch low byte register */
    UART_IER = 1, /* interrupt enable register */
    UART_DLH = 1, /* divisor latch high byte register */
    UART_IIR = 2, /* interrupt identification register */
    UART_FCR = 2, /* fifo control register */
    UART_LCR = 3, /* line control register */
    UART_MCR = 4, /* modem control register */
    UART_LSR = 5, /* line status register */
};

static isr_st saved_isr_handler;
extern void *krn_isr_uart;

static uint8_t
krn_uart_is_thr_empty(void)
{
    uint8_t status = inb(UART_BASE + UART_LSR);

    return status & 0x20;
}

global void
krn_uart_putc(char c)
{
    volatile uint32_t i;

    for (i = 0; i < 1000000; ++i) {
        if (!krn_uart_is_thr_empty()) {
            continue;
        }

        outb(c, UART_BASE + UART_THR);
        break;
    }
}

global void
krn_uart_puts(const char *s)
{
    while (*s) {
        if ((*s) == '\n') {
            krn_uart_putc('\r');
        }

        krn_uart_putc(*s++);
    }
}

global void
krn_uart_handle_intr(void)
{
    event_st ev;

    while (inb(UART_BASE + UART_LSR) & 0x01) {
        ev.type = EVENT_UART_RX;
        ev.payload.uart.byte = inb(UART_BASE + UART_RBR);
        (void)krn_event_ipush(&ev);
    }

    outb(0x20, 0x20);
}

global void
krn_uart_init(void)
{
    /* Disable interrupts */
    outb(0x00, UART_BASE + UART_IER);

    /* Enable DLAB (divisor latch access bit) */
    outb(0x80, UART_BASE + UART_LCR);

    /* Set divisor to 0x0030 */
    /* 115200 BPS / 0x0030 = 2400 BPS */
    outb(0x30, UART_BASE + UART_DLL);
    outb(0x00, UART_BASE + UART_DLH);

    /* Disable DLAB, disable parity bit, set one stop bit, set 8-bit word */
    outb(0x03, UART_BASE + UART_LCR);

    /* FCR: enable FIFO, clear RX/TX, 14-byte trigger */
    outb(0xC7, UART_BASE + UART_FCR);

    /* Set DTR | RTS | OUT2 */
    outb(0x0B, UART_BASE + UART_MCR);

    /* Install ISR */
    krn_get_isr(0x0c, &saved_isr_handler);
    krn_set_isr(0x0c, krn_data_seg, (uint16_t)(uint32_t)&krn_isr_uart);

    /* Unmask IRQ4 */
    outb(inb(0x21) & ~(1 << 4), 0x21);

    /* Enable interrupts */
    outb(0x01, UART_BASE + UART_IER);
}

global void
krn_uart_deinit(void)
{
    /* Disable interrupts */
    outb(0x00, UART_BASE + UART_IER);

    /* Mask IRQ4 */
    outb(inb(0x21) | (1 << 4), 0x21);

    /* Restore previous ISR */
    krn_set_isr(0x0c, saved_isr_handler.seg, saved_isr_handler.ofs);
}
