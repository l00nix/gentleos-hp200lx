/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: setup.c - Settings app
 */

#include <gui.h>

enum {
    WINDOW_WIDTH = 200,
    WINDOW_HEIGHT = 120,
};

static window_st window;

static void
on_show(void)
{
    static int initialized = 0;

    if (!initialized) {
        gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

        initialized = 1;
    }

    gui_window_draw(&window, COLOR_BG, 1);
}

global app_st app_setup = {
    "Setup",
    &icon_setup,
    &window,
    on_show,
};
