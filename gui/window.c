/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: window.c - Window routines
 */

#include <gui.h>

global void
gui_window_rect(window_st *window, rect_st *out)
{
    gui_rect_init(out,
        window->origin.x,
        window->origin.y,
        window->size.width,
        window->size.height
    );
}

global void
gui_window_area(window_st *window, rect_st *out)
{
    gui_rect_init(out, 0, 0, window->size.width, window->size.height);
}

global void
gui_window_init(window_st *window, int width, int height)
{
    rect_st rect;

    memset(window, 0, sizeof(*window));

    window->size.width = width;
    window->size.height = height;

    gui_window_area(window, &rect);
    gui_rect_center(&rect, &gui_app_rect);

    window->origin.x = rect.x;
    window->origin.y = rect.y;
}

global void
gui_window_draw(window_st *window, uint8_t bg_color, int border)
{
    rect_st area;
    size_t i;

    gui_window_area(window, &area);
    gui_surface_draw_rect(&window->origin, &area, bg_color);

    if (border) {
        gui_surface_draw_border(&window->origin, &area, COLOR_FG);
    }

    gui_surface_mark_dirty(&window->origin, &area);
}
