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
    &app_mines,
    &app_pairs,
    &app_snake,
    &app_tetris,
    &app_blackjack,
};

#if __CPROTO__
#define APPS_COUNT 1
#else
#define APPS_COUNT (sizeof(apps) / sizeof(apps[0]))
#endif

static window_st window;

static widget_st app_buttons[APPS_COUNT];
static widget_st *widgets[APPS_COUNT];

static void
on_button_press(widget_st *widget)
{
    gui_widget_draw(widget);

    if (apps[widget->tag1]) {
        apps[widget->tag1]->show();
        gui_status_set_tl("GentleOS > %s", apps[widget->tag1]->name);
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
    window.widgets = widgets;
    window.widgets_capacity = sizeof(widgets) / sizeof(widgets[0]);
    window.focused_widget = &app_buttons[0];
}

static void
init_app_buttons(void)
{
    uint16_t i;
    int col, row;

    memset(&app_buttons, 0, sizeof(app_buttons));

    for (i = 0; i < APPS_COUNT; i++) {
        col = i % GRID_COLS;
        row = i / GRID_COLS;

        app_buttons[i].type = WIDGET_TYPE_BUTTON;
        app_buttons[i].rect.x = GRID_X + col * APP_BUTTON_H_STRIDE;
        app_buttons[i].rect.y = GRID_Y + row * APP_BUTTON_V_STRIDE;
        app_buttons[i].rect.width = APP_BUTTON_WIDTH;
        app_buttons[i].rect.height = APP_BUTTON_HEIGHT;
        app_buttons[i].bitmap = apps[i]->icon;
        app_buttons[i].tag1 = i;
        app_buttons[i].on_press = on_button_press;
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
    gui_window_draw(&window);

    gui_status_set_tl("GentleOS");
}

app_st app_launcher = {
    "Launcher",
	0,
    show_app,
};
