/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: surface.c - Surface drawing routines
 */

#include <gui.h>

static uint8_t far *gui_surface_pixels;
static rect_st gui_surface_dirty_rect = { 0 };

global void
gui_surface_init(void)
{
    gui_surface_pixels = krn_heap_alloc(GUI_FB_PLANE_SIZE);
}

global void
gui_surface_clear(void)
{
    memset_far(gui_surface_pixels, (gui_color_bg << 4) | gui_color_bg, GUI_FB_PLANE_SIZE);
    gui_rect_init(&gui_surface_dirty_rect, 0, 0, GUI_WIDTH, GUI_HEIGHT);
}

global void
gui_surface_invert(void)
{
    uint16_t far *words = (uint16_t far *)gui_surface_pixels;
    uint16_t count = GUI_FB_PLANE_SIZE / 2;
    size_t i;

    for (i = 0; i < count; ++i) {
        words[i] = ~words[i];
    }

    gui_surface_mark_dirty(&GUI_POINT_ZERO, &GUI_RECT_SCREEN);
}

global void
gui_surface_mark_dirty(const point_st *origin, const rect_st *rect)
{
    static rect_st screen_rect = { 0, 0, GUI_WIDTH, GUI_HEIGHT };
    rect_st combined;
    rect_st translated;

    gui_rect_copy(&translated, rect);
    gui_rect_translate(&translated, origin);

    gui_rect_copy(&combined, &gui_surface_dirty_rect);
    gui_rect_enclose(&combined, &translated);
    gui_rect_clip(&combined, &screen_rect);

    gui_rect_copy(&gui_surface_dirty_rect, &combined);
}

global void
gui_surface_flush(void)
{
    rect_st rect;
    int x0, x1, byte_x0, byte_count, y;
    uint8_t far *vram = MK_FP(0xb800, 0);

    gui_rect_copy(&rect, &gui_surface_dirty_rect);

    if (gui_rect_is_empty(&rect)) {
        return;
    }

    x0 = (rect.x / 4) * 4;
    x1 = ((rect.x + rect.width + 3) / 4) * 4;
    byte_x0 = x0 / 4;
    byte_count = (x1 - x0) / 4;

    for (y = rect.y; y < rect.y + rect.height; y += 1) {
        memcpy_far(
            vram + (y % 2) * 0x2000 + (y / 2) * GUI_FB_PITCH + byte_x0,
            gui_surface_pixels + y * GUI_FB_PITCH + byte_x0,
            byte_count
        );
    }

    gui_rect_init(&gui_surface_dirty_rect, 0, 0, 0, 0);
}

global void
gui_surface_draw_pixel(const point_st *origin, int x, int y, uint8_t color)
{
    int byte_idx = (origin->y + y) * GUI_FB_PITCH + (origin->x + x) / 4;
    int shift = (3 - ((origin->x + x) & 3)) * 2;
    uint8_t mask = 0x03 << shift;
    uint8_t val = (color & 1) ? mask : 0;

    gui_surface_pixels[byte_idx] = (gui_surface_pixels[byte_idx] & ~mask) | val;
}

global void
gui_surface_draw_h_seg(const point_st *origin, int x, int y, int w, uint8_t color)
{
    rect_st r;
    gui_rect_init(&r, x, y, w, 1);
    gui_surface_draw_rect(origin, &r, color);
}

global void
gui_surface_draw_v_seg(const point_st *origin, int x, int y, int h, uint8_t color)
{
    int i;

    for (i = 0; i < h; i++) {
        gui_surface_draw_pixel(origin, x, y + i, color);
    }
}

global void
gui_surface_draw_border(const point_st *origin, const rect_st *r, uint8_t color)
{
    gui_surface_draw_h_seg(origin, r->x, r->y, r->width, color);
    gui_surface_draw_h_seg(origin, r->x, r->y + r->height - 1, r->width, color);
    gui_surface_draw_v_seg(origin, r->x, r->y, r->height, color);
    gui_surface_draw_v_seg(origin, r->x + r->width - 1, r->y, r->height, color);
}

global void
gui_surface_draw_rect(const point_st *origin, const rect_st *rect, uint8_t color)
{
    rect_st translated;
    int l_x, r_x, l_byte, r_byte;
    uint8_t mask, l_mask, r_mask, fill;
    uint8_t far *dst_plane, far *dst_row;
    int y;

    gui_rect_copy(&translated, rect);
    gui_rect_translate(&translated, origin);

    l_x = translated.x;
    r_x = translated.x + translated.width - 1;

    if (r_x < l_x) {
        return;
    }

    l_byte = l_x / 4;
    r_byte = r_x / 4;

    l_mask = 0xFF >> ((l_x & 3) * 2);
    r_mask = 0xFF << ((3 - (r_x & 3)) * 2);

    fill = (color & 1) ? 0xFF : 0x00;
    dst_plane = gui_surface_pixels;

    for (y = translated.y; y < translated.y + translated.height; ++y) {
        dst_row = dst_plane + y * GUI_FB_PITCH;

        if (l_byte == r_byte) {
            mask = l_mask & r_mask;
            dst_row[l_byte] = (dst_row[l_byte] & ~mask) | (fill & mask);
            continue;
        }

        dst_row[l_byte] = (dst_row[l_byte] & ~l_mask) | (fill & l_mask);

        if (r_byte > l_byte + 1) {
            memset_far(dst_row + l_byte + 1, fill, r_byte - l_byte - 1);
        }

        dst_row[r_byte] = (dst_row[r_byte] & ~r_mask) | (fill & r_mask);
    }
}

global void
gui_surface_scroll_up(const point_st *origin, const rect_st *rect, int dy)
{
    rect_st translated;
    int y, byte_x, byte_count;

    gui_rect_copy(&translated, rect);
    gui_rect_translate(&translated, origin);

    byte_x = translated.x / 4;
    byte_count = translated.width / 4;

    for (y = translated.y; y < translated.y + translated.height - dy; ++y) {
        memcpy_far(
            gui_surface_pixels + y * GUI_FB_PITCH + byte_x,
            gui_surface_pixels + (y + dy) * GUI_FB_PITCH + byte_x,
            byte_count
        );
    }

    gui_surface_mark_dirty(origin, rect);
}

global void
gui_surface_draw_char(const point_st *origin, uint16_t x, uint16_t y,
    font_st *font, uint8_t ch, uint8_t fg, uint8_t bg)
{
    const uint8_t *glyph;
    uint8_t fg_bit, bg_bit;
    uint8_t glyph_byte, target_byte;
    int i, j, bit;

    if (!font) {
        font = &fonts[0];
    }

    if (!ch) {
        ch = ' ';
    }

    if (ch > 127) {
        ch = '\x08';
    }

    glyph = font->pixels + (ch * font->size.height);

    fg_bit = fg & 1;
    bg_bit = bg & 1;

    for (j = 0; j < font->size.height; ++j) {
        glyph_byte = glyph[j];

        if (fg_bit && !bg_bit) {
            target_byte = glyph_byte;
        } else if (!fg_bit && bg_bit) {
            target_byte = ~glyph_byte;
        } else if (fg_bit && bg_bit) {
            target_byte = 0xFF;
        } else {
            target_byte = 0x00;
        }

        for (i = 0; i < font->size.width; ++i) {
            bit = (target_byte >> (7 - i)) & 1;
            gui_surface_draw_pixel(origin, x + i, y + j, bit);
        }
    }
}

global void
gui_surface_draw_str(const point_st *origin, uint16_t x, uint16_t y,
    font_st *font, const char *s, uint8_t fg, uint8_t bg)
{
    int i;

    if (!font) {
        font = &fonts[0];
    }

    for (i = 0; s[i]; i++) {
        gui_surface_draw_char(origin, x + i * font->size.width, y, font, s[i], fg, bg);
    }
}

global void
gui_surface_draw_str_centered(const point_st *origin, const rect_st *rect,
    font_st *font, const char *s, uint8_t fg, uint8_t bg)
{
    int x, y, text_width;

    if (!font) {
        font = &fonts[0];
    }

    text_width = (uint16_t)(strlen(s)) * font->size.width;

    x = rect->x + (rect->width - text_width) / 2;
    y = rect->y + (rect->height - font->size.height) / 2;

    gui_surface_draw_str(origin, x, y, font, s, fg, bg);
}

global void
gui_surface_draw_bitmap(const point_st *origin, const size_st *bounds, int dst_x, int dst_y,
    bitmap_st *bitmap, uint8_t fill)
{
    rect_st src_rect;
    uint8_t fill_bit;
    uint16_t i, j;

    gui_rect_init(&src_rect, 0, 0, bitmap->size.width, bitmap->size.height);

    if (dst_x + src_rect.width > bounds->width) {
        src_rect.width = bounds->width - dst_x;
        src_rect.width = src_rect.width < 0 ? 0 : src_rect.width;
    }

    if (dst_y + src_rect.height > bounds->height) {
        src_rect.height = bounds->height - dst_y;
        src_rect.height = src_rect.height < 0 ? 0 : src_rect.height;
    }

    fill_bit = fill & 1;

    for (i = 0; i < src_rect.height; i++) {
        for (j = 0; j < src_rect.width; j++) {
            int byte_no = (src_rect.y + i) * bitmap->pitch + (src_rect.x + j) / 8;
            int bit_no = 7 - ((src_rect.x + j) % 8);
            int active = (bitmap->pixels[byte_no] >> bit_no) & 1;

            if (!active) {
                continue;
            }

            gui_surface_draw_pixel(origin, dst_x + j, dst_y + i, fill_bit);
        }
    }
}

global void
gui_surface_draw_bitmap_centered(const point_st *origin, const size_st *bounds, const rect_st *rect,
    bitmap_st *bitmap, uint8_t fill)
{
    int x = rect->x + (rect->width - bitmap->size.width) / 2;
    int y = rect->y + (rect->height - bitmap->size.height) / 2;

    gui_surface_draw_bitmap(origin, bounds, x, y, bitmap, fill);
}
