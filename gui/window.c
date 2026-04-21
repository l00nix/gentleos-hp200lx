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
gui_window_draw(window_st *window)
{
    rect_st area;
    size_t i;

    gui_window_area(window, &area);
    gui_surface_draw_rect(&window->origin, &area, window->bg_color);

    if (!window->hide_border) {
        gui_surface_draw_border(&window->origin, &area, COLOR_FG);
    }

    for (i = 0; i < window->widgets_count; ++i) {
        gui_widget_draw(window->widgets[i]);
    }

    gui_surface_mark_dirty(&window->origin, &area);
}

global int
gui_window_add_widget(window_st *window, widget_st *widget)
{
    if (window->widgets_count >= window->widgets_capacity) {
        return -1;
    }

    widget->window = window;
    window->widgets[window->widgets_count++] = widget;

    return 0;
}

global void
gui_window_on_close(window_st *window)
{
    if (window->on_close) {
        window->on_close(window);
    }
}

global void
gui_window_on_key_down(window_st *window, const event_st *event)
{
    int key_code = event->payload.key.key_code;
    int key_char = event->payload.key.key_char;

    if (key_code == KEY_ESC) {
        gui_run_app(&app_launcher);
        return;
    }

    if (key_char == 'Q') {
        krn_exit();
        return;
    }

    if (window->on_key_down) {
        window->on_key_down(window, event);
        return;
    }
}

global void
gui_window_on_key_up(window_st *window, const event_st *event)
{
    if (window->on_key_up) {
        window->on_key_up(window, event);
    }
}

global void
gui_window_on_uart_rx(window_st *window, const event_st *event)
{
    if (window->on_uart_rx) {
        window->on_uart_rx(window, event);
    }
}

global void
gui_window_on_tick(window_st *window)
{
    if (window->on_tick) {
        window->on_tick(window);
    }
}
