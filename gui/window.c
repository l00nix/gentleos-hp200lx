/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: window.c - Window routines
 */

#include <gui.h>

rect_st
gui_window_rect(window_st *window)
{
    return (rect_st) {
        .pos = window->origin,
        .size = window->size,
    };
}

rect_st
gui_window_area(window_st *window)
{
    return (rect_st) {
        .x = 0,
        .y = 0,
        .width = window->size.width,
        .height = window->size.height,
    };
}

void
gui_window_init_frame(window_st *window)
{
    window->origin = gui_rect_center(gui_window_area(window), gui_wm_container).pos;
}

void
gui_window_draw(window_st *window)
{
    gui_surface_draw_border(window->origin, gui_window_area(window), COLOR_FG);

    rect_st title_rect = {
        .x = 0,
        .y = 0,
        .width = window->size.width,
        .height = TITLE_BAR_HEIGHT,
    };

    gui_surface_draw_border(window->origin, title_rect, COLOR_FG);
    gui_surface_draw_rect(window->origin, gui_rect_shrink(title_rect, 1), COLOR_BG);
    gui_surface_draw_str_centered(window->origin, title_rect,
        font_8x8, window->title, COLOR_FG, COLOR_BG);

    rect_st content_rect = {
        .x = 1,
        .y = TITLE_BAR_HEIGHT + 1,
        .width = window->size.width - 2,
        .height = window->size.height - TITLE_BAR_HEIGHT - 2,
    };
    gui_surface_draw_rect(window->origin, content_rect, window->bg_color);

    for (size_t i = 0; i < window->widgets_count; ++i) {
        gui_widget_draw(window->widgets[i]);
    }
}

int
gui_window_add_widget(window_st *window, widget_st *widget)
{
    if (window->widgets_count >= window->widgets_capacity) {
        return -1;
    }

    widget->window = window;
    window->widgets[window->widgets_count++] = widget;

    return 0;
}

void
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

    if (!current_widget) {
        return;
    }

    int cur_x = current_widget->focus_x;
    int cur_y = current_widget->focus_y;

    widget_st *best_widget = NULL;
    int min_fwd_dist = 0xffffff;
    int min_lat_dist = 0xffffff;

    for (size_t i = 0; i < window->widgets_count; ++i) {
        widget_st *w = window->widgets[i];

        if (!w->focusable || w == current_widget) {
            continue;
        }

        int diff_x = w->focus_x - cur_x;
        int diff_y = w->focus_y - cur_y;

        int fwd_dist = dir_x ? (diff_x * dir_x) : (diff_y * dir_y);
        int lat_dist = dir_x ? ABS(diff_y) : ABS(diff_x);

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
    }
}

void
gui_window_on_key_down(window_st *window, event_st event)
{
    widget_st *focused = window->focused_widget;

    if (focused) {
        switch (event.payload.key.key_code) {
            case KEY_LEFT:  gui_window_update_focus(window, -1, 0); return;
            case KEY_RIGHT: gui_window_update_focus(window, 1, 0); return;
            case KEY_UP:    gui_window_update_focus(window, 0, -1); return;
            case KEY_DOWN:  gui_window_update_focus(window, 0, 1); return;
        }

        if (event.payload.key.key_code == KEY_ENTER && focused->on_pointer_up) {
            focused->on_pointer_up(focused, event, (point_st){0, 0});
            return;
        }
    }

    if (event.payload.key.key_code == KEY_ESC) {
        app_launcher.show();
    }

    if (window->on_key_down) {
        window->on_key_down(window, event);
    }
}
