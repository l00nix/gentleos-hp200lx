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

    gui_status_set("");
    gui_wm_render_desktop_region(gui_wm_container, NULL);

    return 0;
}

static void
gui_wm_render_wallpaper(rect_st rect)
{
    gui_fb_draw_start();
    if (gui_wm_bg_pattern) {
        gui_fb_draw_pattern(rect, gui_wm_bg_pattern, COLOR_FG, COLOR_BG);
    } else {
        gui_fb_draw_rect(rect, COLOR_BG);
    }
    gui_fb_draw_end();
}

void
gui_wm_render_window_surface(window_st *window, rect_st desktop_reg)
{
    desktop_reg = gui_rect_clip(desktop_reg, window->rect);
    rect_st window_reg = gui_rect_translate_back(desktop_reg, window->rect.pos);

    gui_fb_draw_start();
    gui_fb_draw_surface(desktop_reg.x, desktop_reg.y, window->surface, window_reg);
    gui_fb_draw_end();
}

// Re-render a specified region of the desktop to the screen,
// by rendering that region in all windows from the bottom up,
// starting from a specified bottom window
void
gui_wm_render_desktop_region(rect_st rect, window_st *bottom_window)
{
    if (!bottom_window) {
        gui_wm_render_wallpaper(rect);
    }

    if (gui_wm_current_window) {
        gui_wm_render_window_surface(gui_wm_current_window, rect);
    }
}

void
gui_wm_render_window_region(window_st *window, rect_st window_reg)
{
    rect_st desktop_reg;

    if (!window->visible) {
        return;
    }

    desktop_reg = gui_rect_translate(window_reg, window->rect.pos);

    if (window == gui_wm_status_window) {
        gui_wm_render_window_surface(window, desktop_reg);
    } else {
        gui_wm_render_desktop_region(desktop_reg, window);
    }
}

window_st *
gui_wm_find_window(uint16_t x, uint16_t y)
{
    point_st p = { .x = x, .y = y };

    if (gui_wm_current_window && gui_rect_contains_point(gui_wm_current_window->rect, p)) {
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
