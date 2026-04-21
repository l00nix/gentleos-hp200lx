/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: terminal.c - Serial terminal app
 */

#include <gui.h>

enum {
    MARGIN = 4,

    COLS = 72,
    ROWS = 18,
    CHAR_W = 4,
    CHAR_H = 8,

    WINDOW_WIDTH = MARGIN * 2 + COLS * CHAR_W,
    WINDOW_HEIGHT = MARGIN * 2 + ROWS * CHAR_H,

    LOCAL_ECHO = 1,

    /* Timer runs at 20 Hz (50 ms per tick) */
    CURSOR_BLINK_TICKS = 10,
};

/* Must match CHAR_W & CHAR_H */
#define CHAR_FONT FONT_4x8

static window_st window;
static int cursor_col;
static int cursor_row;
static int cursor_visible;

static void
cell_rect_init(rect_st *out, int col, int row)
{
    gui_rect_init(out, MARGIN + col * CHAR_W, MARGIN + row * CHAR_H, CHAR_W, CHAR_H);
}

static void
set_cursor_visible(int visible)
{
    rect_st r;
    uint8_t color = visible ? COLOR_FG : COLOR_BG;

    if (cursor_visible == visible) {
        return;
    }

    cell_rect_init(&r, cursor_col, cursor_row);
    r.y += CHAR_H - 1;
    r.height = 1;

    gui_surface_draw_rect(&window.origin, &r, color);
    gui_surface_mark_dirty(&window.origin, &r);

    cursor_visible = visible;
}

static void
draw_char(int col, int row, char c)
{
    rect_st r;

    cell_rect_init(&r, col, row);
    gui_surface_draw_char(&window.origin, r.x, r.y, CHAR_FONT, c, COLOR_FG, COLOR_BG);
    gui_surface_mark_dirty(&window.origin, &r);
}

static void
clear_char(int col, int row)
{
    rect_st r;

    cell_rect_init(&r, col, row);
    gui_surface_draw_rect(&window.origin, &r, COLOR_BG);
    gui_surface_mark_dirty(&window.origin, &r);
}

static void
scroll_up(void)
{
    rect_st text_area, bottom_row;

    gui_rect_init(&text_area, MARGIN, MARGIN, COLS * CHAR_W, ROWS * CHAR_H);
    gui_surface_scroll_up(&window.origin, &text_area, CHAR_H);

    gui_rect_init(&bottom_row, MARGIN, MARGIN + (ROWS - 1) * CHAR_H,
        COLS * CHAR_W, CHAR_H);
    gui_surface_draw_rect(&window.origin, &bottom_row, COLOR_BG);
}

static void
advance_row(void)
{
    cursor_row++;

    if (cursor_row >= ROWS) {
        scroll_up();
        cursor_row = ROWS - 1;
    }
}

static void
put_char(uint8_t c)
{
    if (c == '\r') {
        cursor_col = 0;
        return;
    }

    if (c == '\n') {
        cursor_col = 0;
        advance_row();
        return;
    }

    if (c == '\b') {
        if (cursor_col > 0) {
            cursor_col--;
            clear_char(cursor_col, cursor_row);
        }
        return;
    }

    if (c < 32 || c > 126) {
        return;
    }

    draw_char(cursor_col, cursor_row, c);
    cursor_col++;

    if (cursor_col >= COLS) {
        cursor_col = 0;
        advance_row();
    }
}

static void
send_char(uint8_t c)
{
    krn_uart_putc(c);

    if (LOCAL_ECHO) {
        put_char(c);
    }
}

static void
on_uart_rx(window_st *win, const event_st *event)
{
    set_cursor_visible(0);
    put_char(event->payload.uart.byte);
    set_cursor_visible(1);
}

static void
on_key_down(window_st *win, const event_st *event)
{
    uint8_t ch = event->payload.key.key_char;

    set_cursor_visible(0);

    if (ch == '\n') {
        send_char('\r');
        send_char('\n');
    } else if ((ch >= 32 && ch <= 126) || ch == '\b') {
        send_char(ch);
    }

    set_cursor_visible(1);
}

static void
on_tick(window_st *win)
{
    static unsigned cursor_blink_count = 0;

    if (++cursor_blink_count < CURSOR_BLINK_TICKS) {
        return;
    }

    cursor_blink_count = 0;

    set_cursor_visible(!cursor_visible);
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    /* Align window to 4px boundary to use gui_surface_scroll_up */
    window.origin.x &= ~3;

    window.bg_color = COLOR_BG;
    window.on_key_down = on_key_down;
    window.on_uart_rx = on_uart_rx;
    window.on_tick = on_tick;
}

static void
on_show(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_window();
        initialized = 1;
    }

    gui_window_draw(&window);

    cursor_col = 0;
    cursor_row = ROWS - 1;
    cursor_visible = 0;
    set_cursor_visible(1);
}

global app_st app_terminal = {
    "Serial Terminal",
    &icon_serial,
    &window,
    on_show,
};
