// --------------------------------------------------------------------------------------
// Copyright (c) 2025-2026 luke8086
// Distributed under the terms of GPL-2 License
// --------------------------------------------------------------------------------------
// File: surface.c - Surface drawing routines (1bpp planar)
// --------------------------------------------------------------------------------------

#include <gui.h>

static uint8_t *gui_surface_fb_pixels = NULL;
static int gui_surface_fb_pitch = 0;

static uint8_t gui_surface_pixels[GUI_FB_PLANE_SIZE] __attribute__((aligned(16)));
static rect_st gui_surface_dirty_rect = { 0 };

void
gui_surface_init(void)
{
    gui_surface_fb_pixels = krn_core_mboot_info->fb_addr;
    gui_surface_fb_pitch = krn_core_mboot_info->fb_pitch;
}

void
gui_surface_mark_dirty(rect_st rect)
{
    static rect_st screen_rect = { .width = GUI_WIDTH, .height = GUI_HEIGHT };

    rect = gui_rect_enclose(gui_surface_dirty_rect, rect);
    rect = gui_rect_clip(rect, screen_rect);

    gui_surface_dirty_rect = rect;
}

void
gui_surface_flush(void)
{
    rect_st rect = gui_surface_dirty_rect;

    if (gui_rect_is_empty(rect)) {
        return;
    }

    int x0 = (rect.x / 8) * 8;
    int x1 = ((rect.x + rect.width + 7) / 8) * 8;
    int byte_x0 = x0 / 8;
    int byte_count = (x1 - x0) / 8;

    for (int y = rect.y; y < rect.y + rect.height; ++y) {
        memcpy(
            gui_surface_fb_pixels + y * gui_surface_fb_pitch + byte_x0,
            gui_surface_pixels + y * GUI_FB_PITCH + byte_x0,
            byte_count
        );
    }

    gui_surface_dirty_rect = GUI_RECT_ZERO;
}

static inline void
gui_surface_draw_pixel(int x, int y, uint8_t color)
{
    int byte_idx = y * GUI_FB_PITCH + x / 8;
    int bit = 7 - (x & 7);

    gui_surface_pixels[byte_idx] &= ~(1 << bit);
    gui_surface_pixels[byte_idx] |= ((color & 1) << bit);
}

void
gui_surface_draw_h_seg(point_st origin, int x, int y, int w, uint8_t color)
{
    rect_st r = { .x = x, .y = y, .width = w, .height = 1 };
    gui_surface_draw_rect(origin, r, color);
}

void
gui_surface_draw_v_seg(point_st origin, int x, int y, int h, uint8_t color)
{
    int sx = origin.x + x;
    int sy = origin.y + y;

    for (int i = 0; i < h; i++) {
        gui_surface_draw_pixel(sx, sy + i, color);
    }
}

void
gui_surface_draw_border(point_st origin, rect_st r, uint8_t color)
{
    gui_surface_draw_h_seg(origin, r.x, r.y, r.width, color);
    gui_surface_draw_h_seg(origin, r.x, r.y + r.height - 1, r.width, color);
    gui_surface_draw_v_seg(origin, r.x, r.y, r.height, color);
    gui_surface_draw_v_seg(origin, r.x + r.width - 1, r.y, r.height, color);
}

void
gui_surface_draw_rect(point_st origin, rect_st rect, uint8_t color)
{
    rect = gui_rect_translate(rect, origin);

    int l_x = rect.x;
    int r_x = rect.x + rect.width - 1;

    if (r_x < l_x) {
        return;
    }

    int l_byte = l_x / 8;
    int r_byte = r_x / 8;

    uint8_t l_mask = 0xFF >> (l_x & 7);
    uint8_t r_mask = 0xFF << (7 - (r_x & 7));

    uint8_t fill = (color & 1) ? 0xFF : 0x00;
    uint8_t *dst_plane = gui_surface_pixels;

    for (int y = rect.y; y < rect.y + rect.height; ++y) {
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
gui_surface_draw_char(point_st origin, uint16_t x, uint16_t y,
    font_st *font, uint8_t ch, uint8_t fg, uint8_t bg)
{
    const uint8_t *glyph;

    if (!ch) {
        ch = ' ';
    }

    glyph = font->pixels + (ch * font->size.height);

    uint8_t fg_bit = fg & 1;
    uint8_t bg_bit = bg & 1;

    for (int j = 0; j < font->size.height; ++j) {
        uint8_t glyph_byte = glyph[j];
        uint8_t target_byte;

        if (fg_bit && !bg_bit) {
            target_byte = glyph_byte;
        } else if (!fg_bit && bg_bit) {
            target_byte = ~glyph_byte;
        } else if (fg_bit && bg_bit) {
            target_byte = 0xFF;
        } else {
            target_byte = 0x00;
        }

        int target_x = origin.x + x;
        int target_y = origin.y + y + j;

        for (int i = 0; i < font->size.width; ++i) {
            int bit = (target_byte >> (7 - i)) & 1;
            gui_surface_draw_pixel(target_x + i, target_y, bit);
        }
    }
}

void
gui_surface_draw_str(point_st origin, uint16_t x, uint16_t y,
    font_st *font, const char *s, uint8_t fg, uint8_t bg)
{
    for (int i = 0; s[i]; i++) {
        gui_surface_draw_char(origin, x + i * font->size.width, y, font, s[i], fg, bg);
    }
}

void
gui_surface_draw_str_centered(point_st origin, rect_st rect,
    font_st *font, const char *s, uint8_t fg, uint8_t bg)
{
    int text_width = strlen(s) * font->size.width;

    int x = rect.x + (rect.width - text_width) / 2;
    int y = rect.y + (rect.height - font->size.height) / 2;

    gui_surface_draw_str(origin, x, y, font, s, fg, bg);
}

void
gui_surface_draw_bitmap(point_st origin, size_st bounds, int dst_x, int dst_y,
    bitmap_st *bitmap, uint8_t fill)
{
    rect_st src_rect = {
        .x = 0,
        .y = 0,
        .size = bitmap->size,
    };

    if (dst_x + src_rect.width > bounds.width) {
        src_rect.width = bounds.width - dst_x;
        src_rect.width = src_rect.width < 0 ? 0 : src_rect.width;
    }

    if (dst_y + src_rect.height > bounds.height) {
        src_rect.height = bounds.height - dst_y;
        src_rect.height = src_rect.height < 0 ? 0 : src_rect.height;
    }

    int target_x = origin.x + dst_x;
    int target_y = origin.y + dst_y;
    uint8_t fill_bit = fill & 1;

    for (uint16_t i = 0; i < src_rect.height; i++) {
        for (uint16_t j = 0; j < src_rect.width; j++) {
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
gui_surface_draw_bitmap_centered(point_st origin, size_st bounds, rect_st rect,
    bitmap_st *bitmap, uint8_t fill)
{
    int x = rect.x + (rect.width - bitmap->size.width) / 2;
    int y = rect.y + (rect.height - bitmap->size.height) / 2;

    gui_surface_draw_bitmap(origin, bounds, x, y, bitmap, fill);
}
