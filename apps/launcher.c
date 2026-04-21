/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: launcher.c - App launcher
 */

#include <gui.h>

enum {
    GRID_COLS = 5,
    GRID_ROWS = 3,

    APP_BUTTON_H_MARGIN = 12,
    APP_BUTTON_V_MARGIN = 8,
    APP_BUTTON_WIDTH = 42,
    APP_BUTTON_HEIGHT = 42,
    APP_BUTTON_H_STRIDE = APP_BUTTON_WIDTH + APP_BUTTON_H_MARGIN,
    APP_BUTTON_V_STRIDE = APP_BUTTON_HEIGHT + APP_BUTTON_V_MARGIN,

    GRID_X = 1 + APP_BUTTON_H_MARGIN,
    GRID_Y = 1 + APP_BUTTON_V_MARGIN,
    GRID_WIDTH = GRID_COLS * APP_BUTTON_H_STRIDE - APP_BUTTON_H_MARGIN,
    GRID_HEIGHT = GRID_ROWS * APP_BUTTON_V_STRIDE - APP_BUTTON_V_MARGIN,

    WINDOW_WIDTH = GRID_X + GRID_WIDTH + APP_BUTTON_H_MARGIN + 1,
    WINDOW_HEIGHT = GRID_Y + GRID_HEIGHT + APP_BUTTON_V_MARGIN + 1,
};

static app_st *apps[] = {
    &app_clock,
    &app_calendar,
    &app_calc,
    &app_fonts,
    &app_keymap,
    &app_sounds,
    &app_terminal,
    &app_mines,
    &app_pairs,
    &app_mahjong,
    &app_snake,
    &app_tetris,
    &app_freecell,
    &app_blackjack,
    &app_setup,
};

#define APPS_COUNT (sizeof(apps) / sizeof(apps[0]))

static window_st window;
static int current_col = 0;
static int current_row = 0;

static void
cell_rect_init(rect_st *out, int col, int row)
{
    out->x = GRID_X + col * APP_BUTTON_H_STRIDE;
    out->y = GRID_Y + row * APP_BUTTON_V_STRIDE;
    out->width = APP_BUTTON_WIDTH;
    out->height = APP_BUTTON_HEIGHT;
}

static void
draw_cell(int col, int row)
{
    rect_st rect, cur_rect;
    int i = row * GRID_COLS + col;

    cell_rect_init(&rect, col, row);
    gui_surface_draw_rect(&window.origin, &rect, COLOR_BG);
    gui_surface_draw_border(&window.origin, &rect, COLOR_FG);

    if (i < APPS_COUNT && apps[i] && apps[i]->icon) {
        gui_surface_draw_bitmap_centered(&window.origin, &window.size, &rect,
            apps[i]->icon, COLOR_FG);
    }

    if (col == current_col && row == current_row) {
        gui_rect_copy(&cur_rect, &rect);
        gui_rect_shrink(&cur_rect, 1);
        gui_surface_draw_border(&window.origin, &cur_rect, COLOR_FG);
    }

    gui_surface_mark_dirty(&window.origin, &rect);
}

static void
draw_all_cells(void)
{
    int col, row;

    for (row = 0; row < GRID_ROWS; ++row) {
        for (col = 0; col < GRID_COLS; ++col) {
            draw_cell(col, row);
        }
    }
}

static void
update_current_cell(int dx, int dy)
{
    int prev_col = current_col;
    int prev_row = current_row;

    current_col = MAX(0, MIN(GRID_COLS - 1, current_col + dx));
    current_row = MAX(0, MIN(GRID_ROWS - 1, current_row + dy));

    draw_cell(prev_col, prev_row);
    draw_cell(current_col, current_row);
}

static void
launch_current_app(void)
{
    int i = current_row * GRID_COLS + current_col;

    if (i < APPS_COUNT && apps[i]) {
        gui_status_set_tl("GentleOS > %s", apps[i]->name);
        gui_run_app(apps[i]);
    }
}

static void
on_key_down(window_st *win, const event_st *event)
{
    int key_code = event->payload.key.key_code;

    switch (key_code) {
        case KEY_LEFT: update_current_cell(-1, 0); return;
        case KEY_RIGHT: update_current_cell(1, 0); return;
        case KEY_UP: update_current_cell(0, -1); return;
        case KEY_DOWN: update_current_cell(0, 1); return;
        case KEY_SPACE:
        case KEY_ENTER: launch_current_app(); return;
    }
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.size.width = WINDOW_WIDTH;
    window.size.height = WINDOW_HEIGHT;
    window.hide_border = 1;
    window.bg_color = COLOR_BG;
    window.on_key_down = on_key_down;
}

static void
show_app(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_window();
        initialized = 1;
    }

    gui_window_draw(&window);
    draw_all_cells();

    gui_status_set_tl("GentleOS");
}

global app_st app_launcher = {
    "Launcher",
	0,
    &window,
    show_app,
};
