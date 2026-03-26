// --------------------------------------------------------------------------------------
// Copyright (c) 2025-2026 luke8086
// Distributed under the terms of GPL-2 License
// --------------------------------------------------------------------------------------
// File: wm.c - Window manager
// --------------------------------------------------------------------------------------

#include <gui.h>

enum {
    WINDOWS_COUNT_MAX = 6,
};

rect_st gui_wm_container = { 0 };

static window_st *gui_wm_status_window = NULL;
static window_st *gui_wm_current_window = NULL;

bitmap_st *gui_wm_bg_pattern = NULL;

static void
gui_wm_render_wallpaper(rect_st rect)
{
    gui_surface_draw_rect(GUI_POINT_ZERO, rect, COLOR_BG);
    gui_surface_mark_dirty(rect);
}

int
gui_wm_add_window(struct window *w)
{
    if (gui_wm_current_window) {
        gui_wm_current_window->visible = 0;
        gui_wm_current_window->active = 0;
        gui_window_on_close(gui_wm_current_window);
        gui_wm_current_window = NULL;
    }

    gui_wm_current_window = w;
    gui_wm_current_window->visible = 1;
    gui_wm_current_window->active = 1;

    gui_wm_render_wallpaper(gui_wm_container);
    gui_status_set("");

    return 0;
}

void
gui_wm_render_window_region(window_st *window, rect_st window_reg)
{
    gui_surface_mark_dirty(gui_rect_translate(window_reg, window->origin));
}

window_st *
gui_wm_find_window(uint16_t x, uint16_t y)
{
    point_st p = { .x = x, .y = y };

    if (gui_wm_current_window && gui_rect_contains_point(gui_window_rect(gui_wm_current_window), p)) {
        return gui_wm_current_window;
    }

    return NULL;
}

window_st *
gui_wm_top_window(void)
{
    return gui_wm_current_window;
}

void
gui_wm_set_status_window(window_st *w)
{
    gui_wm_status_window = w;
    gui_wm_render_window_region(w, gui_window_area(w));
}

void
gui_wm_init(void)
{
    gui_wm_container.width = GUI_WIDTH;
    gui_wm_container.height = GUI_HEIGHT - STATUS_HEIGHT;
    gui_wm_render_wallpaper(gui_wm_container);

    gui_status_init();

    app_launcher.show();
}
