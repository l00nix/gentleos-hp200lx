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
    FONT_WIDTH = 8,
    FONT_HEIGHT = 16,
};

enum {
    TEXT_X = FONT_WIDTH,
    TEXT_Y = (STATUS_HEIGHT - FONT_HEIGHT) / 2,
    TEXT_MAX_LEN = (STATUS_WIDTH / FONT_WIDTH) - 2,
};

static window_st window;

static uint16_t status_text_len = 0;
static uint8_t status_bg_color = 0;
static char status_text_buf[TEXT_MAX_LEN + 1];

static void
gui_status_set_bg_color(uint8_t color)
{
    rect_st bg_rect;

    gui_rect_init(&bg_rect, 0, 1, STATUS_WIDTH, STATUS_HEIGHT - 1);

    if (color == status_bg_color) {
        return;
    }

    gui_surface_draw_rect(&window.origin, &bg_rect, color);
    gui_wm_render_window_region(&window, &bg_rect);

    status_bg_color = color;
}

static void
gui_status_set_text(const char *text, uint8_t color)
{
    uint16_t len = strlen(text);
    font_st *font = font_8x8;
    rect_st clear_rect, text_rect;

    gui_surface_draw_str(&window.origin, TEXT_X, TEXT_Y, font, text, color,
        status_bg_color);

    /* If the new text is shorter than previous, clear the remaining space */
    if (len < status_text_len) {
        gui_rect_init(&clear_rect,
            TEXT_X + len * font->size.width,
            TEXT_Y,
            (status_text_len - len) * font->size.width,
            font->size.height
        );

        gui_surface_draw_rect(&window.origin, &clear_rect, status_bg_color);
    }

    gui_rect_init(&text_rect,
        TEXT_X,
        TEXT_Y,
        STATUS_WIDTH - TEXT_X * 2,
        font->size.height
    );

    gui_wm_render_window_region(&window, &text_rect);

    status_text_len = len;
}

void
gui_status_set(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    (void) vsnprintf(status_text_buf, sizeof(status_text_buf), fmt, args);
    va_end(args);

    gui_status_set_bg_color(COLOR_BG);
    gui_status_set_text(status_text_buf, COLOR_FG);
}


void
gui_status_set_alert(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    (void) vsnprintf(status_text_buf, sizeof(status_text_buf), fmt, args);
    va_end(args);

    gui_status_set_bg_color(COLOR_FG);
    gui_status_set_text(status_text_buf, COLOR_BG);
}

void
gui_status_init(void)
{
    memset(status_text_buf, 0, sizeof(status_text_buf));

    window.size.width = STATUS_WIDTH;
    window.size.height = STATUS_HEIGHT;
    window.origin.x = 0;
    window.origin.y = GUI_HEIGHT - STATUS_HEIGHT;
    window.visible = 1;

    gui_surface_draw_h_seg(&window.origin, 0, 0, STATUS_WIDTH, COLOR_FG);

    gui_status_set("", COLOR_FG);

    gui_wm_set_status_window(&window);
}
