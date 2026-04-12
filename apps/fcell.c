/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: fcell.c - FreeCell game
 */

#include <gui.h>

enum {
    WINDOW_WIDTH = 200,
    WINDOW_HEIGHT = 120
};

static window_st window;

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.bg_color = COLOR_BG;
}

static void
show_app(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_window();
        initialized = 1;
    }

    gui_wm_add_window(&window);
    gui_window_draw(&window);
}

global app_st app_freecell = {
    "FreeCell",
    &icon_fcell,
    show_app,
};
