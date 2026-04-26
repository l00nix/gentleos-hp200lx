/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: window.c - Window routines
 */

#include <gui.h>

global void
gui_window_init(window_st *window, int width, int height)
{
    rect_st rect;

    rect.width = width;
    rect.height = height;
    gui_rect_center(&rect, &gui_app_rect);

    window->size.width = rect.width;
    window->size.height = rect.height;
    window->origin.x = rect.x;
    window->origin.y = rect.y;
}

global void
gui_window_draw(window_st *window, uint8_t bg_color, int border)
{
    rect_st rect;

    gui_rect_init(&rect, 0, 0, window->size.width, window->size.height);

    gui_surface_draw_rect(&window->origin, &rect, bg_color);

    if (border) {
        gui_surface_draw_border(&window->origin, &rect, gui_color_fg);
    }

    gui_surface_mark_dirty(&window->origin, &rect);
}
