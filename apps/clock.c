/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: clock.c - Clock app
 */

#include <gui.h>

enum {
    GRID_CELL_WIDTH = 8,
    GRID_CELL_HEIGHT = 8,
    GRID_COLS = 27,
    GRID_ROWS = 5,
    GRID_X = 1,
    GRID_Y = 1,
    GRID_WIDTH = GRID_WIDTH_SPACED(GRID_CELL_WIDTH, GRID_COLS),
    GRID_HEIGHT = GRID_HEIGHT_SPACED(GRID_CELL_HEIGHT, GRID_ROWS),

    WINDOW_WIDTH = GRID_X + GRID_WIDTH + 1,
    WINDOW_HEIGHT = GRID_Y + GRID_HEIGHT + 1,
};

static window_st window;

static grid_st grid;

static time_st last_time;

static void
draw_cell(int x, int y, int active)
{
    rect_st r;
    uint8_t color = active ? COLOR_FG : COLOR_BG;
    gui_grid_cell_rect(&grid, x, y, &r);
    gui_surface_draw_rect(&window.origin, &r, color);
    gui_surface_mark_dirty(&window.origin, &r);
}

static void
draw_digit(int x, int y, int digit)
{
    static const uint16_t digit_pixels[10] = {
        0xf6de, 0x592e, 0xe7ce, 0xe79e, 0xb792,
        0xf39e, 0xf3de, 0xe492, 0xf7de, 0xf79e,
    };
    int i;
    uint8_t active;

    for (i = 0; i < 15; ++i) {
        active = !!(digit_pixels[digit] & (1 << (15 - i)));
        draw_cell(x + i % 3, y + i / 3, active);
    }
}

static void
draw_time(void)
{
    time_st t;

    bios_get_time(&t);

    if (t.hour == last_time.hour && t.minute == last_time.minute &&
        t.second == last_time.second) {
        return;
    }

    memcpy(&last_time, &t, sizeof(last_time));

    draw_digit(0, 0, t.hour / 10);
    draw_digit(4, 0, t.hour % 10);
    draw_cell(8, 1, 1);
    draw_cell(8, 3, 1);
    draw_digit(10, 0, t.minute / 10);
    draw_digit(14, 0, t.minute % 10);
    draw_cell(18, 1, 1);
    draw_cell(18, 3, 1);
    draw_digit(20, 0, t.second / 10);
    draw_digit(24, 0, t.second % 10);
}

static void
on_tick(window_st *window)
{
    static unsigned count = 0;

    ++count;

    if (count >= 10) {
        count = 0;
        draw_time();
    }
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.bg_color = COLOR_BG;
    window.hide_border = 1;
    window.on_tick = on_tick;
}

static void
init_grid(void)
{
    grid.cell_width = GRID_CELL_WIDTH;
    grid.cell_height = GRID_CELL_HEIGHT;
    grid.cols = GRID_COLS;
    grid.rows = GRID_ROWS;
    grid.x = GRID_X;
    grid.y = GRID_Y;
}

static void
show_app(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_window();
        init_grid();
        initialized = 1;
    }

    gui_window_draw(&window);
    draw_time();
}

global app_st app_clock = {
    "Clock",
    &icon_clock,
    &window,
    show_app,
};
