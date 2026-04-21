/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: fonts.c - Font browser app
 */

#include <gui.h>

enum {
    TOOL_BAR_Y = 0,
    TOOL_BAR_HEIGHT = 24,

    GRID_CELL_WIDTH = 13,
    GRID_CELL_HEIGHT = 14,
    GRID_ROWS = 8,
    GRID_COLS = 16,
    GRID_CELLS_COUNT = (GRID_ROWS * GRID_COLS),
    GRID_WIDTH = GRID_WIDTH_SPACED(GRID_CELL_WIDTH, GRID_COLS),
    GRID_HEIGHT = GRID_HEIGHT_SPACED(GRID_CELL_HEIGHT, GRID_ROWS),
    GRID_X = 1,
    GRID_Y = TOOL_BAR_Y + TOOL_BAR_HEIGHT,

    WINDOW_WIDTH = GRID_X + GRID_WIDTH + 1,
    WINDOW_HEIGHT = GRID_Y + GRID_HEIGHT + 1,
};

static window_st window;
static grid_st grid;

static unsigned current_font;
static unsigned current_col;
static unsigned current_row;

static void
update_status(void)
{
    int n = current_row * grid.rows + current_col;
    gui_status_set("hex:%02x dec:%03d", n, n);
}

static void
draw_cell(int col, int row)
{
    char str[2];
    rect_st r;
    int is_active = col == current_col && row == current_row;

    str[0] = row * grid.cols + col;
    str[1] = 0;

    gui_grid_cell_rect(&grid, col, row, &r);

    gui_surface_draw_rect(&window.origin, &r, is_active ? COLOR_FG : COLOR_BG);

    gui_surface_draw_str_centered(
        &window.origin,
        &r,
        &fonts[current_font],
        (const char *)str,
        is_active ? COLOR_BG : COLOR_FG,
        is_active ? COLOR_FG : COLOR_BG
    );

    gui_surface_mark_dirty(&window.origin, &r);
}

static void
draw_all_cells(void)
{
    int row, col;

    for (row = 0; row < grid.rows; ++row) {
        for (col = 0; col < grid.cols; ++col) {
            draw_cell(col, row);
        }
    }
}

static void
draw_font_label(void)
{
    rect_st r;
    rect_st shrunken;

    gui_rect_init(&r, 0, TOOL_BAR_Y, WINDOW_WIDTH, TOOL_BAR_HEIGHT);

    gui_surface_draw_border(&window.origin, &r, COLOR_FG);
    gui_rect_copy(&shrunken, &r);
    gui_rect_shrink(&shrunken, 1);
    gui_surface_draw_rect(&window.origin, &shrunken, COLOR_BG);
    gui_surface_draw_str_centered(&window.origin, &r, NULL,
        fonts[current_font].name, COLOR_FG, COLOR_BG);

    gui_surface_mark_dirty(&window.origin, &r);
}

static void
set_prev_font(void)
{
    current_font = (current_font - 1) % FONT_COUNT;

    draw_font_label();
    draw_all_cells();
}

static void
set_next_font(void)
{
    current_font = (current_font + 1) % FONT_COUNT;

    draw_font_label();
    draw_all_cells();
}

static void
update_current_cell(int dx, int dy)
{
    int prev_col = current_col;
    int prev_row = current_row;

    current_col = (current_col + dx) % grid.cols;
    current_row = (current_row + dy) % grid.rows;

    draw_cell(prev_col, prev_row);
    draw_cell(current_col, current_row);

    update_status();
}

static void
on_key_down(window_st *window, const event_st *event)
{
    int key_code = event->payload.key.key_code;

    switch (key_code) {
        case KEY_LEFT:  update_current_cell(-1, 0); return;
        case KEY_RIGHT: update_current_cell(1, 0); return;
        case KEY_UP:    update_current_cell(0, -1); return;
        case KEY_DOWN:  update_current_cell(0, 1); return;
        case KEY_PGUP:  set_prev_font(); return;
        case KEY_PGDN:  set_next_font(); return;
    }
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.bg_color = COLOR_FG;
    window.on_key_down = on_key_down;
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
on_show(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_window();
        init_grid();
        initialized = 1;
    }

    gui_window_draw(&window);

    current_font = 0;
    current_col = 0;
    current_row = 0;

    draw_font_label();
    draw_all_cells();
    gui_status_set_br("PgUp/PgDn: Select font");

    update_status();
}

global app_st app_fonts = {
    "Fonts",
    &icon_fonts,
    &window,
    on_show,
};
