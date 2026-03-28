/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: surface.c - Surface drawing routines
 */

#include <gui.h>

static uint8_t gui_surface_pixels[GUI_FB_PLANE_SIZE];
static rect_st gui_surface_dirty_rect = { 0 };

void
gui_surface_init(void)
{
    gui_rect_init(&gui_surface_dirty_rect, 0, 0, 0, 0);
    memset(gui_surface_pixels, 0, sizeof(gui_surface_pixels));
}

void
gui_surface_mark_dirty(const rect_st *rect)
{
    static rect_st screen_rect = { 0, 0, GUI_WIDTH, GUI_HEIGHT };
    rect_st combined;

    gui_rect_copy(&combined, &gui_surface_dirty_rect);
    gui_rect_enclose(&combined, rect);
    gui_rect_clip(&combined, &screen_rect);

    gui_rect_copy(&gui_surface_dirty_rect, &combined);
}

void
gui_surface_flush(void)
{
    rect_st rect;
    int x0, x1, byte_x0, byte_count, y;
    uint8_t far *vram = MK_FP(GUI_VIDEO_SEG, 0);

    gui_rect_copy(&rect, &gui_surface_dirty_rect);

    if (gui_rect_is_empty(&rect)) {
        return;
    }

    x0 = (rect.x / 8) * 8;
    x1 = ((rect.x + rect.width + 7) / 8) * 8;
    byte_x0 = x0 / 8;
    byte_count = (x1 - x0) / 8;

    for (y = rect.y; y < rect.y + rect.height; y += 1) {
#if GUI_VIDEO_INTERLEAVED
        memcpy_far(
            vram + (y % 2) * 0x2000 + (y / 2) * GUI_FB_PITCH + byte_x0,
            gui_surface_pixels + y * GUI_FB_PITCH + byte_x0,
            byte_count
        );
#else
        memcpy_far(
            vram + y * GUI_FB_PITCH + byte_x0,
            gui_surface_pixels + y * GUI_FB_PITCH + byte_x0,
            byte_count
        );
#endif
    }

    gui_rect_copy(&gui_surface_dirty_rect, &GUI_RECT_ZERO);
}

static void
gui_surface_draw_pixel(int x, int y, uint8_t color)
{
    int byte_idx = y * GUI_FB_PITCH + x / 8;
    int bit = 7 - (x & 7);

    gui_surface_pixels[byte_idx] &= ~(1 << bit);
    gui_surface_pixels[byte_idx] |= ((color & 1) << bit);
}

void
gui_surface_draw_h_seg(const point_st *origin, int x, int y, int w, uint8_t color)
{
    rect_st r;
    gui_rect_init(&r, x, y, w, 1);
    gui_surface_draw_rect(origin, &r, color);
}

void
gui_surface_draw_v_seg(const point_st *origin, int x, int y, int h, uint8_t color)
{
    int i;
    int sx = origin->x + x;
    int sy = origin->y + y;

    for (i = 0; i < h; i++) {
        gui_surface_draw_pixel(sx, sy + i, color);
    }
}

void
gui_surface_draw_border(const point_st *origin, const rect_st *r, uint8_t color)
{
    gui_surface_draw_h_seg(origin, r->x, r->y, r->width, color);
    gui_surface_draw_h_seg(origin, r->x, r->y + r->height - 1, r->width, color);
    gui_surface_draw_v_seg(origin, r->x, r->y, r->height, color);
    gui_surface_draw_v_seg(origin, r->x + r->width - 1, r->y, r->height, color);
}

void
gui_surface_draw_rect(const point_st *origin, const rect_st *rect, uint8_t color)
{
    rect_st translated;
    int l_x, r_x, l_byte, r_byte;
    uint8_t l_mask, r_mask, fill, *dst_plane;
    int y;

    gui_rect_copy(&translated, rect);
    gui_rect_translate(&translated, origin);

    l_x = translated.x;
    r_x = translated.x + translated.width - 1;

    if (r_x < l_x) {
        return;
    }

    l_byte = l_x / 8;
    r_byte = r_x / 8;

    l_mask = 0xFF >> (l_x & 7);
    r_mask = 0xFF << (7 - (r_x & 7));

    fill = (color & 1) ? 0xFF : 0x00;
    dst_plane = gui_surface_pixels;

    for (y = translated.y; y < translated.y + translated.height; ++y) {
        uint8_t *dst_row = dst_plane + y * GUI_FB_PITCH;

        if (l_byte == r_byte) {
            uint8_t mask = l_mask & r_mask;
            dst_row[l_byte] = (dst_row[l_byte] & ~mask) | (fill & mask);
            continue;
        }

        dst_row[l_byte] = (dst_row[l_byte] & ~l_mask) | (fill & l_mask);

        if (r_byte > l_byte + 1) {
            memset(dst_row + l_byte + 1, fill, r_byte - l_byte - 1);
        }

        dst_row[r_byte] = (dst_row[r_byte] & ~r_mask) | (fill & r_mask);
    }
}

void
gui_surface_draw_char(const point_st *origin, uint16_t x, uint16_t y,
    font_st *font, uint8_t ch, uint8_t fg, uint8_t bg)
{
    const uint8_t *glyph;
    uint8_t fg_bit, bg_bit;
    uint8_t glyph_byte, target_byte;
    int target_x, target_y;
    int i, j, bit;

    if (!ch) {
        ch = ' ';
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

        target_x = origin->x + x;
        target_y = origin->y + y + j;

        for (i = 0; i < font->size.width; ++i) {
            bit = (target_byte >> (7 - i)) & 1;
            gui_surface_draw_pixel(target_x + i, target_y, bit);
        }
    }
}

void
gui_surface_draw_str(const point_st *origin, uint16_t x, uint16_t y,
    font_st *font, const char *s, uint8_t fg, uint8_t bg)
{
    int i;

    for (i = 0; s[i]; i++) {
        gui_surface_draw_char(origin, x + i * font->size.width, y, font, s[i], fg, bg);
    }
}

void
gui_surface_draw_str_centered(const point_st *origin, const rect_st *rect,
    font_st *font, const char *s, uint8_t fg, uint8_t bg)
{
    int text_width = (uint16_t)(strlen(s)) * font->size.width;

    int x = rect->x + (rect->width - text_width) / 2;
    int y = rect->y + (rect->height - font->size.height) / 2;

    gui_surface_draw_str(origin, x, y, font, s, fg, bg);
}

void
gui_surface_draw_bitmap(const point_st *origin, const size_st *bounds, int dst_x, int dst_y,
    bitmap_st *bitmap, uint8_t fill)
{
    rect_st src_rect;
    int target_x, target_y;
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

    target_x = origin->x + dst_x;
    target_y = origin->y + dst_y;
    fill_bit = fill & 1;

    for (i = 0; i < src_rect.height; i++) {
        for (j = 0; j < src_rect.width; j++) {
            int byte_no = (src_rect.y + i) * bitmap->pitch + (src_rect.x + j) / 8;
            int bit_no = 7 - ((src_rect.x + j) % 8);
            int active = (bitmap->pixels[byte_no] >> bit_no) & 1;

            if (!active) {
                continue;
            }

            gui_surface_draw_pixel(target_x + j, target_y + i, fill_bit);
        }
    }
}

void
gui_surface_draw_bitmap_centered(const point_st *origin, const size_st *bounds, const rect_st *rect,
    bitmap_st *bitmap, uint8_t fill)
{
    int x = rect->x + (rect->width - bitmap->size.width) / 2;
    int y = rect->y + (rect->height - bitmap->size.height) / 2;

    gui_surface_draw_bitmap(origin, bounds, x, y, bitmap, fill);
}
