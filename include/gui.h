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

#define FONT_5x8 (&fonts[0])
#define FONT_4x8 (&fonts[1])
#define FONT_4x6 (&fonts[2])

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

struct widget;
typedef struct widget widget_st;

struct widget {
    point_st *origin;
    rect_st rect;

    int tag1;
    int tag2;

    int active;
    int hide_border;

    void (*draw)(widget_st *);

    const char *label;
};

typedef struct {
    point_st origin;
    size_st size;
} window_st;

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
    void (*on_show)(void);
    void (*on_key_down)(const event_st *);
    void (*on_key_up)(const event_st *);
    void (*on_close)(void);
    void (*on_tick)(void);
} app_st;

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
    GUI_FB_PITCH = GUI_WIDTH / 8,
    GUI_FB_PLANE_SIZE = GUI_HEIGHT * GUI_FB_PITCH,
    GUI_VRAM_PITCH = GUI_WIDTH / 4,
};

#define GRID_WIDTH_SPACED(cell_width, cols) ((cell_width) * (cols) + (cols) - 1)
#define GRID_HEIGHT_SPACED(cell_height, rows) ((cell_height) * (rows) + (rows) - 1)

#include "p_gui.h"
#include "p_data.h"
#include "p_apps.h"

#endif /* _GUI_H_ */
