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

    GRID_CELL_WIDTH = 8,
    GRID_CELL_HEIGHT = 10,
    GRID_ROWS = 8,
    GRID_COLS = 32,
    GRID_CELLS_COUNT = (GRID_ROWS * GRID_COLS),
    GRID_WIDTH = GRID_WIDTH_SPACED(GRID_CELL_WIDTH, GRID_COLS),
    GRID_HEIGHT = GRID_HEIGHT_SPACED(GRID_CELL_HEIGHT, GRID_ROWS),
    GRID_X = 1,
    GRID_Y = TOOL_BAR_Y + TOOL_BAR_HEIGHT,

    WINDOW_WIDTH = GRID_X + GRID_WIDTH + 1,
    WINDOW_HEIGHT = GRID_Y + GRID_HEIGHT + 1,
};

static window_st window;

static widget_st char_buttons[GRID_CELLS_COUNT];
static widget_st *widgets[GRID_CELLS_COUNT];

static size_t current_font = 0;

static grid_st grid;

static void
update_status(void)
{
    widget_st *btn = window.focused_widget;

    if (!btn) {
        gui_status_set("");
        return;
    }

    gui_status_set("hex:%02x dec:%03d", btn->tag2, btn->tag2);
}

static void
draw_char_button(widget_st *widget)
{
    char str[2];
    int is_active = widget == widget->window->focused_widget;

    str[0] = widget->tag2 ? widget->tag2 : ' ';
    str[1] = 0;

    gui_surface_draw_rect(&window.origin, &widget->rect,
        is_active ? COLOR_FG : COLOR_BG);

    gui_surface_draw_str_centered(
        &window.origin,
        &widget->rect,
        &fonts[current_font],
        (const char *)str,
        is_active ? COLOR_BG : COLOR_FG,
        is_active ? COLOR_FG : COLOR_BG
    );

    gui_wm_render_window_region(&widget->window->origin, &widget->rect);
}

static void
draw_all_char_buttons(void)
{
    size_t i;

    for (i = 0; i < GRID_CELLS_COUNT; ++i) {
        char_buttons[i].draw(&char_buttons[i]);
    }
}

static void
draw_font_label(void)
{
    rect_st r;
    rect_st shrunken;

    gui_rect_init(&r, 1, TOOL_BAR_Y, WINDOW_WIDTH - 2, TOOL_BAR_HEIGHT);

    gui_surface_draw_border(&window.origin, &r, COLOR_FG);
    gui_rect_copy(&shrunken, &r);
    gui_rect_shrink(&shrunken, 1);
    gui_surface_draw_rect(&window.origin, &shrunken, COLOR_BG);
    gui_surface_draw_str_centered(&window.origin, &r, NULL,
        fonts[current_font].name, COLOR_FG, COLOR_BG);

    gui_wm_render_window_region(&window.origin, &r);
}

static void
set_prev_font(void)
{
    current_font = (current_font - 1) % FONT_COUNT;

    draw_font_label();
    draw_all_char_buttons();
}

static void
set_next_font(void)
{
    current_font = (current_font + 1) % FONT_COUNT;

    draw_font_label();
    draw_all_char_buttons();
}

static void
on_focus_changed(window_st *window)
{
    update_status();
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.bg_color = COLOR_FG;
    window.widgets = widgets;
    window.widgets_capacity = sizeof(widgets) / sizeof(widgets[0]);
    window.focused_widget = &char_buttons[0];
    window.on_focus_changed = on_focus_changed;
}

static void
init_char_buttons(void)
{
    uint16_t i;
    int col, row;

    grid.cell_width = GRID_CELL_WIDTH;
    grid.cell_height = GRID_CELL_HEIGHT;
    grid.cols = GRID_COLS;
    grid.rows = GRID_ROWS;
    grid.x = GRID_X;
    grid.y = GRID_Y;

    memset(char_buttons, 0, sizeof(char_buttons));

    for (i = 0; i < GRID_CELLS_COUNT; ++i) {
        col = i % grid.cols;
        row = i / grid.cols;

        char_buttons[i].type = WIDGET_TYPE_BUTTON;
        gui_grid_cell_rect(&grid, col, row, &char_buttons[i].rect);
        char_buttons[i].tag2 = i;
        char_buttons[i].window = &window;
        char_buttons[i].draw = draw_char_button;
        char_buttons[i].focusable = 1;
        char_buttons[i].focus_x = col;
        char_buttons[i].focus_y = row;

        gui_window_add_widget(&window, &char_buttons[i]);
    }
}

static void
show_app(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_window();
        init_char_buttons();
        initialized = 1;
    }

    gui_wm_add_window(&window);
    gui_window_draw(&window);
    draw_font_label();
}

app_st app_fonts = {
    "Fonts",
    &bitmap_icon_fonts,
    show_app,
};
