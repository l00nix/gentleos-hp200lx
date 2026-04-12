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

enum {
    CORNER_TL = 0,
    CORNER_TR = 1,
    CORNER_BL = 2,
    CORNER_BR = 3,
};

static uint16_t status_text_len[4];
static char status_text_buf[TEXT_MAX_LEN + 1];

static void
gui_status_set_text(int corner, const char *text)
{
    point_st origin = { 0, 0 };
    rect_st clear_rect, text_rect;
    uint16_t len = strlen(text);
    font_st *font = &fonts[0];
    int width = len * font->size.width;
    int prev_width = status_text_len[corner] * font->size.width;
    int x = TEXT_X;
    int prev_x = TEXT_X;

    if (corner == CORNER_BL || corner == CORNER_BR) {
        origin.y = GUI_HEIGHT - STATUS_HEIGHT;
    }

    if (corner == CORNER_TR || corner == CORNER_BR) {
        x = STATUS_WIDTH - TEXT_X - width;
        prev_x = STATUS_WIDTH - TEXT_X - prev_width;
    }

    gui_surface_draw_str(&origin, x, TEXT_Y, font, text, COLOR_FG, COLOR_BG);

    /* If the new text is shorter than previous, clear the remaining space */
    if (len < status_text_len[corner]) {
        gui_rect_init(&clear_rect,
            (corner == CORNER_TR || corner == CORNER_BR) ? prev_x : x + width,
            TEXT_Y,
            prev_width - width,
            font->size.height
        );

        gui_surface_draw_rect(&origin, &clear_rect, COLOR_BG);
    }

    gui_rect_init(&text_rect, TEXT_X, 0, STATUS_WIDTH - TEXT_X * 2, STATUS_HEIGHT);
    gui_wm_render_window_region(&origin, &text_rect);

    status_text_len[corner] = len;
}

global void
gui_status_set_tl(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    (void) vsnprintf(status_text_buf, sizeof(status_text_buf), fmt, args);
    va_end(args);

    gui_status_set_text(CORNER_TL, status_text_buf);
}

global void
gui_status_set_tr(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    (void) vsnprintf(status_text_buf, sizeof(status_text_buf), fmt, args);
    va_end(args);

    gui_status_set_text(CORNER_TR, status_text_buf);
}

global void
gui_status_set(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    (void) vsnprintf(status_text_buf, sizeof(status_text_buf), fmt, args);
    va_end(args);

    gui_status_set_text(CORNER_BL, status_text_buf);
}

global void
gui_status_set_br(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    (void) vsnprintf(status_text_buf, sizeof(status_text_buf), fmt, args);
    va_end(args);

    gui_status_set_text(CORNER_BR, status_text_buf);
}

global void
gui_status_init(void)
{
    point_st origin = { 0, 0 };
    size_st size = { GUI_WIDTH, GUI_HEIGHT };
    memset(status_text_len, 0, sizeof(status_text_len));
    memset(status_text_buf, 0, sizeof(status_text_buf));

    gui_surface_draw_h_seg(&origin, 0, STATUS_HEIGHT - 1, STATUS_WIDTH, COLOR_FG);
    gui_surface_draw_h_seg(&origin, 0, GUI_HEIGHT - STATUS_HEIGHT, STATUS_WIDTH, COLOR_FG);
    gui_surface_draw_bitmap(&origin, &size,
        STATUS_WIDTH - 2 * TEXT_X - 19 * FONT_WIDTH - sprite_github.size.width,
        1, &sprite_github, COLOR_FG);

    gui_status_set("");
    gui_status_set_tl("");
    gui_status_set_tr("luke8086/gentleos16");
}
