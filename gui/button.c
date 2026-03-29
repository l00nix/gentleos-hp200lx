/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: button.c - Button widget
 */

#include <gui.h>

void
gui_button_draw(widget_st *widget)
{
    rect_st rect = widget->rect;
    rect_st full_rect = rect;

    int is_pressed = widget->active;
    int is_focused = (widget == widget->window->focused_widget);

    point_st origin = widget->window->origin;

    if (!widget->hide_border) {
        gui_surface_draw_border(origin, rect, COLOR_FG);
        rect = gui_rect_shrink(rect, 1);
    }

    if (is_focused && !is_pressed) {
        gui_surface_draw_border(origin, rect, COLOR_FG);
        rect = gui_rect_shrink(rect, 1);
    }

    gui_surface_draw_rect(origin, rect, is_pressed ? COLOR_FG : COLOR_BG);

    if (widget->bitmap) {
        gui_surface_draw_bitmap_centered(
            origin,
            widget->window->size,
            rect,
            widget->bitmap,
            is_pressed ? COLOR_BG : COLOR_FG
        );
    } else if (widget->label) {
        gui_surface_draw_str_centered(
            origin,
            rect,
            widget->font ? widget->font : font_8x8,
            widget->label,
            is_pressed ? COLOR_BG : COLOR_FG,
            is_pressed ? COLOR_FG : COLOR_BG
        );
    }

    gui_wm_render_window_region(widget->window, full_rect);
}
