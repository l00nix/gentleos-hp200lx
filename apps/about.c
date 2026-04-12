/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: about.c - System info app
 */

#include <gui.h>

enum {
    GRID_CELL_WIDTH = 4,
    GRID_CELL_HEIGHT = 9,
    GRID_ROWS = 8,
    GRID_COLS = 33,
    GRID_CELLS_COUNT = (GRID_ROWS * GRID_COLS),
    GRID_WIDTH = GRID_WIDTH_SPACED(GRID_CELL_WIDTH, GRID_COLS),
    GRID_HEIGHT = GRID_HEIGHT_SPACED(GRID_CELL_HEIGHT, GRID_ROWS),
    GRID_X = 1,
    GRID_Y = 1,

    WINDOW_WIDTH = GRID_X + GRID_WIDTH + 1,
    WINDOW_HEIGHT = GRID_Y + GRID_HEIGHT + 1,

    LABEL_COL = 2,
    VALUE_COL = 11,
    VALUE_LEN = GRID_COLS - VALUE_COL - 2,
};

static window_st window;

static grid_st grid;

static void
draw_text_lg(int col, int row, const char *text)
{
    rect_st r;

    if (row >= GRID_COLS) {
        return;
    }

    gui_grid_cell_rect(&grid, col, row, &r);
    gui_surface_draw_str(&window.origin, r.x, r.y, NULL,
        text, COLOR_FG, COLOR_BG);
}

static void
draw_text_sm(int col, int row, const char *text)
{
    rect_st r;

    if (row >= GRID_COLS) {
        return;
    }

    gui_grid_cell_rect(&grid, col, row, &r);
    gui_surface_draw_str(&window.origin, r.x, r.y, NULL,
        text, COLOR_FG, COLOR_BG);
}

/*
static void
draw_github_line(void)
{
    const char *text = "   luke8086/gentleos";
    rect_st r;

    int col = (GRID_COLS - strlen(text)) / 2;
    int line = GRID_ROWS - 2;

    draw_text_sm(col, line, text);

    gui_grid_cell_rect(&grid, col, line, &r);
    r.y -= 5;
    r.width = bitmap_i_github.size.width;
    r.height = bitmap_i_github.size.height;
    gui_surface_draw_bitmap_centered(&window.origin, &window.size, &r, &bitmap_i_github,
        COLOR_FG);
}
*/

static void
draw_info(void)
{
    rect_st r;
    int line = 0;
    const char *title = "-=[ GENTLE OS ]=-";

    gui_grid_rect(&grid, &r);
    gui_surface_draw_rect(&window.origin, &r, window.bg_color);

    line++;
    draw_text_lg((GRID_COLS - strlen(title)) / 2, line++, title);
    line++;

    /* draw_github_line(); */

    gui_wm_render_window_region(&window.origin, &r);
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.bg_color = COLOR_BG;
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

    gui_wm_add_window(&window);
    gui_window_draw(&window);
    draw_info();
}

global app_st app_about = {
    "About",
    &icon_about,
    show_app,
};
