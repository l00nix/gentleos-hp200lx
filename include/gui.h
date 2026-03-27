/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: gui.h - GUI API
 */

#ifndef _GUI_H_
#define _GUI_H_

#include <kernel.h>

typedef struct {
    int x;
    int y;
} point_st;

typedef struct {
    int width;
    int height;
} size_st;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} rect_st;

enum {
    FONT_COUNT = 1,
};

typedef struct {
    size_st size;
    const char *name;
    const uint8_t *pixels;
} font_st;

typedef struct {
    size_st size;
    int pitch;
    const uint8_t *pixels;
} bitmap_st;

enum {
    WIDGET_TYPE_UNKNOWN = 0,
    WIDGET_TYPE_BUTTON = 1,
};

struct window;
typedef struct window window_st;

struct widget;
typedef struct widget widget_st;

struct widget {
    window_st *window;
    rect_st rect;

    int type;
    int tag1;
    int tag2;

    int active;
    int press_on_move_in;
    int press_sticky;
    int press_fixed;
    int hidden;
    int hide_border;
    int focusable;
    int focus_x;
    int focus_y;

    void (*draw)(widget_st *);
    void (*on_pointer_down)(widget_st *, const event_st *, const point_st *);
    void (*on_pointer_up)(widget_st *, const event_st *, const point_st *);
    void (*on_pointer_out)(widget_st *, const event_st *, const point_st *);
    void (*on_pointer_move)(widget_st *, const event_st *, const point_st *);
    void (*on_pointer_alt)(widget_st *, const event_st *, const point_st *);

    const char *label;
    font_st *font;
    bitmap_st *bitmap;
};

struct window {
    point_st origin;
    size_st size;

    int visible;
    int active;

    const char *title;
    uint8_t bg_color;

    widget_st **widgets;
    size_t widgets_count;
    size_t widgets_capacity;
    widget_st *focused_widget;

    void (*render_region)(window_st *, const rect_st *);
    void (*on_pointer)(window_st *, const event_st *);
    void (*on_key_down)(window_st *, const event_st *);
    void (*on_key_up)(window_st *, const event_st *);
    void (*on_close)(window_st *);
};

typedef struct {
    int cell_width;
    int cell_height;
    int cols;
    int rows;
    int x;
    int y;
} grid_st;

typedef struct {
    bitmap_st *icon;
    void (*show)(void);
} app_st;

typedef void *timeout_payload;
typedef void (*timeout_callback_fn)(timeout_payload);

enum {
    COLOR_BG = 0x00,
    COLOR_FG = 0x0f,
};

enum {
    KEY_UP = 0x48,
    KEY_DOWN = 0x50,
    KEY_LEFT = 0x4b,
    KEY_RIGHT = 0x4d,
    KEY_ENTER = 0x1c,
    KEY_ESC = 0x01,
};

enum {
    TITLE_BAR_HEIGHT = 24,
    STATUS_HEIGHT = 24,
};


enum {
    GUI_FB_PITCH = GUI_WIDTH / 8,
    GUI_FB_PLANE_SIZE = GUI_HEIGHT * GUI_FB_PITCH,
};

#define GRID_WIDTH_SPACED(cell_width, cols) ((cell_width) * (cols) + (cols) - 1)
#define GRID_HEIGHT_SPACED(cell_height, rows) ((cell_height) * (rows) + (rows) - 1)

#include "p_gui.h"
#include "p_data.h"
#include "p_apps.h"

#endif /* _GUI_H_ */
