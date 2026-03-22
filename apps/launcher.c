// --------------------------------------------------------------------------------------
// Copyright (c) 2026 luke8086
// Distributed under the terms of GPL-2 License
// --------------------------------------------------------------------------------------
// File: launcher.c - App launcher
// --------------------------------------------------------------------------------------

#include <gui.h>

enum {
    GRID_COLS = 5,
    GRID_ROWS = 3,

    APP_BUTTON_MARGIN = 8,
    APP_BUTTON_SIZE = 48,
    APP_BUTTON_STRIDE = APP_BUTTON_SIZE + APP_BUTTON_MARGIN,
    APPS_COUNT = 13,

    GRID_X = 1 + APP_BUTTON_MARGIN,
    GRID_Y = TITLE_BAR_HEIGHT + APP_BUTTON_MARGIN,
    GRID_WIDTH = GRID_COLS * APP_BUTTON_STRIDE - APP_BUTTON_MARGIN,
    GRID_HEIGHT = GRID_ROWS * APP_BUTTON_STRIDE - APP_BUTTON_MARGIN,

    WINDOW_WIDTH = GRID_X + GRID_WIDTH + APP_BUTTON_MARGIN + 1,
    WINDOW_HEIGHT = GRID_Y + GRID_HEIGHT + APP_BUTTON_MARGIN + 1,
};

static app_st *apps[APPS_COUNT] = {
    &app_about,
    &app_clock,
    &app_calendar,
    &app_calc,
    &app_fonts,
    &app_colors,
    &app_patterns,
    &app_sounds,
    &app_snake,
    &app_mines,
    &app_tetris,
    &app_pairs,
    &app_blackjack,
};

static uint8_t window_pixels[WINDOW_WIDTH * WINDOW_HEIGHT];
static surface_st window_surface;
static window_st window;

static widget_st title_bar;
static widget_st app_buttons[APPS_COUNT];
static widget_st *widgets[APPS_COUNT + 1];

static void
on_button_pointer_up(widget_st *widget, event_st event _unsd, point_st pos _unsd)
{
    gui_widget_draw(widget);

    if (apps[widget->tag1]) {
        apps[widget->tag1]->show();
    }
}

static void
init_window(void)
{
    window_surface.size.width = WINDOW_WIDTH;
    window_surface.size.height = WINDOW_HEIGHT;
    window_surface.pitch = WINDOW_WIDTH;
    window_surface.pixels = window_pixels;

    window.surface = &window_surface;
    window.title = "Launcher";
    window.bg_color = COLOR_WINDOW;
    window.widgets = widgets;
    window.widgets_capacity = sizeof(widgets) / sizeof(widgets[0]);
    window.focused_widget = &app_buttons[0];

    gui_window_init_frame(&window, &title_bar);
}

static void
init_app_buttons(void)
{
    for (size_t i = 0; i < APPS_COUNT; i++) {
        int col = i % GRID_COLS;
        int row = i / GRID_COLS;

        app_buttons[i].type = WIDGET_TYPE_BUTTON;
        app_buttons[i].rect.x = GRID_X + col * APP_BUTTON_STRIDE;
        app_buttons[i].rect.y = GRID_Y + row * APP_BUTTON_STRIDE;
        app_buttons[i].rect.width = APP_BUTTON_SIZE;
        app_buttons[i].rect.height = APP_BUTTON_SIZE;
        app_buttons[i].bitmap = apps[i]->icon;
        app_buttons[i].tag1 = i;
        app_buttons[i].on_pointer_up = on_button_pointer_up;
        app_buttons[i].focusable = 1;
        app_buttons[i].focus_x = col;
        app_buttons[i].focus_y = row;

        gui_window_add_widget(&window, &app_buttons[i]);
    }
}

static void
show_app(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_window();
        init_app_buttons();
        initialized = 1;
    }

    gui_wm_add_window(&window);
}

app_st app_launcher = {
    .show = show_app,
};
