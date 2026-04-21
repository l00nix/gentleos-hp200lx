/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: gui.c - GUI main function
 */

#include <gui.h>

global rect_st gui_app_rect;
global app_st *gui_current_app;

global void
gui_run_app(app_st *app)
{
    if (gui_current_app) {
        gui_current_app->window->visible = 0;
        gui_window_on_close(gui_current_app->window);
        gui_current_app = NULL;
    }

    gui_surface_draw_rect(&GUI_POINT_ZERO, &gui_app_rect, COLOR_BG);
    gui_surface_mark_dirty(&GUI_POINT_ZERO, &gui_app_rect);
    gui_status_set("");
    gui_status_set_br("");

    gui_current_app = app;
    gui_current_app->window->visible = 1;
    gui_current_app->on_show();
}

global void
gui_main(void)
{
    event_st event;
    window_st *w;

    gui_app_rect.x = 0;
    gui_app_rect.y = STATUS_HEIGHT;
    gui_app_rect.width = GUI_WIDTH;
    gui_app_rect.height = GUI_HEIGHT - STATUS_HEIGHT * 2;

    gui_surface_init();
    gui_status_init();
    gui_run_app(&app_launcher);
    gui_surface_flush();

    while (1) {
        if (krn_event_count() == 0) {
            krn_timer_is_cpu_idle = 1;
            cpu_hlt();
            krn_timer_is_cpu_idle = 0;
            continue;
        }

        if (krn_event_pop(&event) != 0) {
            continue;
        }

        w = gui_current_app->window;

        if (event.type == EVENT_TIMER_TICK) {
            gui_window_on_tick(w);
        } else if (event.type == EVENT_KEY_DOWN && w) {
            gui_window_on_key_down(w, &event);
        } else if (event.type == EVENT_KEY_UP && w) {
            gui_window_on_key_up(w, &event);
        } else if (event.type == EVENT_UART_RX && w) {
            gui_window_on_uart_rx(w, &event);
        }

        if (krn_event_count() == 0) {
            gui_surface_flush();
        }
    }
}
