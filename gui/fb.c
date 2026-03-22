// --------------------------------------------------------------------------------------
// Copyright (c) 2025-2026 luke8086
// Distributed under the terms of GPL-2 License
// --------------------------------------------------------------------------------------
// File: fb.c - Framebuffer routines
// --------------------------------------------------------------------------------------

#include <gui.h>

static surface_st _gui_fb_vram_surface = { 0 };
surface_st *gui_fb_vram_surface = &_gui_fb_vram_surface;

static rect_st dirty_rect = { 0 };

void
gui_fb_draw_start(void)
{
}

void
gui_fb_draw_end(void)
{
}

void
gui_fb_mark_dirty(rect_st rect)
{
    static rect_st screen_rect = { .width = GUI_WIDTH, .height = GUI_HEIGHT };
    dirty_rect = gui_rect_clip(gui_rect_enclose(dirty_rect, rect), screen_rect);
}

void
gui_fb_draw_rect(rect_st rect, uint8_t color)
{
    gui_planar_draw_rect(rect, color);
    gui_fb_mark_dirty(rect);
}

void
gui_fb_draw_pattern(rect_st rect, bitmap_st *pattern, uint8_t c1, uint8_t c2)
{
    gui_planar_draw_pattern(rect, pattern, c1, c2);
    gui_fb_mark_dirty(rect);
}

void
gui_fb_draw_surface(int dst_x, int dst_y, surface_st *src_sf, rect_st src_rect)
{
    gui_planar_draw_surface(dst_x, dst_y, src_sf, src_rect);
    gui_fb_mark_dirty(gui_rect_make(dst_x, dst_y, src_rect.width, src_rect.height));
}

void
gui_fb_flush(void)
{
    if (gui_rect_is_empty(dirty_rect)) {
        return;
    }

    rect_st rect = dirty_rect;
    dirty_rect = (rect_st) { 0 };

    gui_planar_flush(rect);
}

void
gui_fb_init(void)
{
    gui_fb_vram_surface->size.width = krn_core_mboot_info->fb_width;
    gui_fb_vram_surface->size.height = krn_core_mboot_info->fb_height;
    gui_fb_vram_surface->pitch = krn_core_mboot_info->fb_pitch;
    gui_fb_vram_surface->pixels = krn_core_mboot_info->fb_addr;
}
