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
    gui_rect_center(&rect, &gui_wm_container);

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

static void
gui_window_update_focus(window_st *window, int dir_x, int dir_y)
{
    widget_st *current_widget = window->focused_widget;
    widget_st *best_widget = NULL;
    widget_st *w;
    int cur_x, cur_y;
    int min_fwd_dist = 0xfff;
    int min_lat_dist = 0xfff;
    int diff_x, diff_y;
    int fwd_dist, lat_dist;
    size_t i;

    if (!current_widget) {
        return;
    }

    cur_x = current_widget->focus_x;
    cur_y = current_widget->focus_y;

    for (i = 0; i < window->widgets_count; ++i) {
        w = window->widgets[i];

        if (!w->focusable || w == current_widget) {
            continue;
        }

        diff_x = w->focus_x - cur_x;
        diff_y = w->focus_y - cur_y;

        fwd_dist = dir_x ? (diff_x * dir_x) : (diff_y * dir_y);
        lat_dist = dir_x ? ABS(diff_y) : ABS(diff_x);

        if (fwd_dist <= 0) {
            continue;
        }

        if (fwd_dist < min_fwd_dist ||
            (fwd_dist == min_fwd_dist && lat_dist < min_lat_dist)) {
            best_widget = w;
            min_fwd_dist = fwd_dist;
            min_lat_dist = lat_dist;
        }
    }

    if (best_widget) {
        window->focused_widget = best_widget;
        gui_widget_draw(current_widget);
        gui_widget_draw(best_widget);

        if (window->on_focus_changed) {
            window->on_focus_changed(window);
        }
    }
}

global void
gui_window_on_key_down(window_st *window, const event_st *event)
{
    widget_st *focused = window->focused_widget;
    int key_code = event->payload.key.key_code;
    int key_char = event->payload.key.key_char;

    if (focused) {
        switch (key_code) {
            case KEY_LEFT:  gui_window_update_focus(window, -1, 0); return;
            case KEY_RIGHT: gui_window_update_focus(window, 1, 0); return;
            case KEY_UP:    gui_window_update_focus(window, 0, -1); return;
            case KEY_DOWN:  gui_window_update_focus(window, 0, 1); return;
        }

        if ((key_code == KEY_ENTER || key_char == ' ') && focused->on_press) {
            focused->on_press(focused);
            return;
        }
    }

    if (key_code == KEY_ESC) {
        app_launcher.show();
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
gui_window_on_tick(window_st *window)
{
    if (window->on_tick) {
        window->on_tick(window);
    }
}
