/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: status.c - Status bar routines
 */

#include <gui.h>

enum {
    STATUS_WIDTH = GUI_WIDTH,
};

enum {
    FONT_WIDTH = 5,
    FONT_HEIGHT = 8,
};

enum {
    TEXT_X = FONT_WIDTH,
    TEXT_Y = (STATUS_HEIGHT - FONT_HEIGHT) / 2,
    TEXT_MAX_LEN = (STATUS_WIDTH / FONT_WIDTH) - 2,
};

static point_st origin = { 0, GUI_HEIGHT - STATUS_HEIGHT };
static uint16_t status_text_len = 0;
static char status_text_buf[TEXT_MAX_LEN + 1];

static void
gui_status_set_text(const char *text)
{
    uint16_t len = strlen(text);
    font_st *font = &fonts[0];
    rect_st clear_rect, text_rect;

    gui_surface_draw_str(&origin, TEXT_X, TEXT_Y, font, text, COLOR_FG, COLOR_BG);

    /* If the new text is shorter than previous, clear the remaining space */
    if (len < status_text_len) {
        gui_rect_init(&clear_rect,
            TEXT_X + len * font->size.width,
            TEXT_Y,
            (status_text_len - len) * font->size.width,
            font->size.height
        );

        gui_surface_draw_rect(&origin, &clear_rect, COLOR_BG);
    }

    gui_rect_init(&text_rect,
        TEXT_X,
        TEXT_Y,
        STATUS_WIDTH - TEXT_X * 2,
        font->size.height
    );

    gui_wm_render_window_region(&origin, &text_rect);

    status_text_len = len;
}

void
gui_status_set(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    (void) vsnprintf(status_text_buf, sizeof(status_text_buf), fmt, args);
    va_end(args);

    gui_status_set_text(status_text_buf);
}

void
gui_status_init(void)
{
    memset(status_text_buf, 0, sizeof(status_text_buf));

    gui_surface_draw_h_seg(&origin, 0, 0, STATUS_WIDTH, COLOR_FG);
    gui_status_set("");
}
