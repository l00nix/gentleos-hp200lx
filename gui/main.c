// --------------------------------------------------------------------------------------
// Copyright (c) 2025-2026 luke8086
// Distributed under the terms of GPL-2 License
// --------------------------------------------------------------------------------------
// File: gui.c - GUI main function
// --------------------------------------------------------------------------------------

#include <gui.h>

void
gui_main(void)
{
    event_st event;

    gui_vga_init();
    gui_surface_init();
    gui_wm_init();
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

        if (event.type == EVENT_TIMER_TICK) {
            gui_timeout_on_tick(event);
        } else if (event.type == EVENT_KEY_DOWN) {
            window_st *w = gui_wm_top_window();

            if (w) {
                gui_window_on_key_down(w, event);
            }
        } else if (event.type == EVENT_KEY_UP) {
            window_st *w = gui_wm_top_window();

            if (w && w->on_key_up) {
                w->on_key_up(w, event);
            }
        }

        if (krn_event_count() == 0) {
            gui_surface_flush();
        }
    }
}
