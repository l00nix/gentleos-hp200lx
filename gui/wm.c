/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: wm.c - Window manager
 */

#include <gui.h>

global window_st *gui_wm_current_window = NULL;

static void
gui_wm_render_wallpaper(const rect_st *rect)
{
    gui_surface_draw_rect(&GUI_POINT_ZERO, rect, COLOR_BG);
    gui_surface_mark_dirty(&GUI_POINT_ZERO, rect);
}

global int
gui_wm_add_window(struct window *w)
{
    if (gui_wm_current_window) {
        gui_wm_current_window->visible = 0;
        gui_window_on_close(gui_wm_current_window);
        gui_wm_current_window = NULL;
    }

    gui_wm_current_window = w;
    gui_wm_current_window->visible = 1;

    gui_wm_render_wallpaper(&gui_app_rect);
    gui_status_set("");

    return 0;
}

global void
gui_wm_init(void)
{
    app_launcher.show();
}
