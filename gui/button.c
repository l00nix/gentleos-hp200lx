/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: button.c - Button widget
 */

#include <gui.h>

global void
gui_button_draw(widget_st *widget)
{
    rect_st rect, full_rect;
    int is_pressed;

    gui_rect_copy(&rect, &widget->rect);
    gui_rect_copy(&full_rect, &rect);

    is_pressed = widget->active;

    if (!widget->hide_border) {
        gui_surface_draw_border(widget->origin, &rect, COLOR_FG);
        gui_rect_shrink(&rect, 1);
    }

    gui_surface_draw_rect(widget->origin, &rect, is_pressed ? COLOR_FG : COLOR_BG);

    if (widget->label) {
        gui_surface_draw_str_centered(
            widget->origin,
            &rect,
            NULL,
            widget->label,
            is_pressed ? COLOR_BG : COLOR_FG,
            is_pressed ? COLOR_FG : COLOR_BG
        );
    }

    gui_surface_mark_dirty(widget->origin, &full_rect);
}
