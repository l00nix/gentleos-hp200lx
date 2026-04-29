/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: gui.c - GUI main function
 */

#include <gui.h>

global rect_st gui_app_rect;
global app_st *gui_current_app;

global int gui_colors_inverted;
global uint8_t gui_color_bg;
global uint8_t gui_color_fg;

global void
gui_set_colors_inverted(int inverted)
{
    gui_color_bg = inverted ? 0x0f : 0x00;
    gui_color_fg = inverted ? 0x00 : 0x0f;

    if (inverted == gui_colors_inverted) {
        return;
    }

    gui_colors_inverted = inverted;
    gui_surface_invert();
}

global void
gui_run_app(app_st *app)
{
    if (gui_current_app) {
        gui_current_app = NULL;
    }

    gui_surface_draw_rect(&GUI_POINT_ZERO, &gui_app_rect, gui_color_bg);
    gui_surface_mark_dirty(&GUI_POINT_ZERO, &gui_app_rect);
    gui_status_set("");
    gui_status_set_br("");

    gui_current_app = app;
    gui_current_app->on_show();
}

global void
gui_main(void)
{
    event_st event;
    app_st *app;

    gui_app_rect.x = 0;
    gui_app_rect.y = STATUS_HEIGHT;
    gui_app_rect.width = GUI_WIDTH;
    gui_app_rect.height = GUI_HEIGHT - STATUS_HEIGHT * 2;

    gui_surface_init();
    gui_colors_inverted = DEFAULT_COLORS_INVERTED;
    gui_set_colors_inverted(DEFAULT_COLORS_INVERTED);
    gui_surface_clear();
    gui_status_init();
    gui_run_app(&app_launcher);
    gui_surface_flush();

    while (1) {
        krn_event_wait(&event);

        app = gui_current_app;

        if (event.type == EVENT_TIMER_TICK) {
            if (app->on_tick) {
                app->on_tick();
            }
        } else if (event.type == EVENT_KEY_DOWN) {
            if (event.payload.key.key_code == KEY_ESC) {
                gui_run_app(&app_launcher);
            } else if (event.payload.key.key_char == 'Q') {
                krn_exit();
            } else if (app->on_key_down) {
                app->on_key_down(&event);
            }
        } else if (event.type == EVENT_KEY_UP) {
            if (app->on_key_up) {
                app->on_key_up(&event);
            }
        }

        if (krn_event_count() == 0) {
            gui_surface_flush();
        }
    }
}
