// --------------------------------------------------------------------------------------
// Copyright (c) 2025-2026 luke8086
// Distributed under the terms of GPL-2 License
// --------------------------------------------------------------------------------------
// File: title_bar.c - Window title bar
// --------------------------------------------------------------------------------------

#include <gui.h>

static void
gui_title_bar_draw(widget_st *widget)
{
    char title[64];
    snprintf(title, sizeof(title), "%s", widget->window->title);

    window_st *win = widget->window;
    int bg_color = win->active ? COLOR_TITLE_BAR_ACTIVE : COLOR_TITLE_BAR_INACTIVE;

    gui_surface_draw_border(win->surface, widget->rect, COLOR_BORDER);
    gui_surface_draw_rect(win->surface, gui_rect_shrink(widget->rect, 1), bg_color);
    gui_surface_draw_str_centered(win->surface, widget->rect,
        font_8x16, title, COLOR_TEXT_ACTIVE, bg_color);

    gui_wm_render_window_region(widget->window, widget->rect);
}

void
gui_title_bar_init(widget_st *bar, window_st *window)
{
    bar->rect = (rect_st) {
        .x = 0,
        .y = 0,
        .width = window->surface->size.width,
        .height = TITLE_BAR_HEIGHT,
    };

    bar->press_sticky = 1;

    bar->draw = gui_title_bar_draw;
}
