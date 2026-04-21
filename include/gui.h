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
    FONT_COUNT = 3,
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
    int hide_border;

    void (*draw)(widget_st *);

    const char *label;
};

struct window {
    point_st origin;
    size_st size;

    int visible;

    uint8_t bg_color;
    int hide_border;

    widget_st **widgets;
    size_t widgets_count;
    size_t widgets_capacity;

    void (*on_key_down)(window_st *, const event_st *);
    void (*on_key_up)(window_st *, const event_st *);
    void (*on_uart_rx)(window_st *, const event_st *);
    void (*on_focus_changed)(window_st *);
    void (*on_close)(window_st *);
    void (*on_tick)(window_st *);
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
    const char *name;
    bitmap_st *icon;
    void (*show)(void);
} app_st;

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
    KEY_SPACE = 0x39,
    KEY_ESC = 0x01,
    KEY_PGUP = 0x49,
    KEY_PGDN = 0x51,
};

enum {
    STATUS_HEIGHT = 16,
};

enum {
    GUI_WIDTH = 320,
    GUI_HEIGHT = 200,
    GUI_FB_PITCH = GUI_WIDTH / 4,
    GUI_FB_PLANE_SIZE = GUI_HEIGHT * GUI_FB_PITCH,
};

#define GRID_WIDTH_SPACED(cell_width, cols) ((cell_width) * (cols) + (cols) - 1)
#define GRID_HEIGHT_SPACED(cell_height, rows) ((cell_height) * (rows) + (rows) - 1)

#include "p_gui.h"
#include "p_data.h"
#include "p_apps.h"

#endif /* _GUI_H_ */
