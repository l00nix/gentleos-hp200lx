/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: keys.c - Keyboard map
 */

#include <gui.h>

typedef struct {
    uint16_t code;
    uint8_t width;
    const char *label;
    int x;
    int y;
} key_st;

enum {
    KEY_H = 15,
    KEY_W = 15,
};

static key_st keys[] = {
    { 0x01, KEY_W, "Esc" },
    { 0x3B, KEY_W, "F1" },
    { 0x3C, KEY_W, "F2" },
    { 0x3D, KEY_W, "F3" },
    { 0x3E, KEY_W, "F4" },
    { 0x3F, KEY_W, "F5" },
    { 0x40, KEY_W, "F6" },
    { 0x41, KEY_W, "F7" },
    { 0x42, KEY_W, "F8" },
    { 0x43, KEY_W, "F9" },
    { 0x44, KEY_W, "F10" },
    { 0x57, KEY_W, "F11" },
    { 0x58, KEY_W, "F12" },
    { 0x29, KEY_W, "`" },
    { 0x02, KEY_W, "1" },
    { 0x03, KEY_W, "2" },
    { 0x04, KEY_W, "3" },
    { 0x05, KEY_W, "4" },
    { 0x06, KEY_W, "5" },
    { 0x07, KEY_W, "6" },
    { 0x08, KEY_W, "7" },
    { 0x09, KEY_W, "8" },
    { 0x0A, KEY_W, "9" },
    { 0x0B, KEY_W, "0" },
    { 0x0C, KEY_W, "-" },
    { 0x0D, KEY_W, "=" },
    { 0x0E, 34, "Bksp" },
    { 0x0F, 23, "Tab" },
    { 0x10, KEY_W, "Q" },
    { 0x11, KEY_W, "W" },
    { 0x12, KEY_W, "E" },
    { 0x13, KEY_W, "R" },
    { 0x14, KEY_W, "T" },
    { 0x15, KEY_W, "Y" },
    { 0x16, KEY_W, "U" },
    { 0x17, KEY_W, "I" },
    { 0x18, KEY_W, "O" },
    { 0x19, KEY_W, "P" },
    { 0x1A, KEY_W, "[" },
    { 0x1B, KEY_W, "]" },
    { 0x2B, 26, "\\" },
    { 0x3A, 27, "Caps" },
    { 0x1E, KEY_W, "A" },
    { 0x1F, KEY_W, "S" },
    { 0x20, KEY_W, "D" },
    { 0x21, KEY_W, "F" },
    { 0x22, KEY_W, "G" },
    { 0x23, KEY_W, "H" },
    { 0x24, KEY_W, "J" },
    { 0x25, KEY_W, "K" },
    { 0x26, KEY_W, "L" },
    { 0x27, KEY_W, ";" },
    { 0x28, KEY_W, "'" },
    { 0x1C, 37, "Enter" },
    { 0x2A, 35, "Shift" },
    { 0x2C, KEY_W, "Z" },
    { 0x2D, KEY_W, "X" },
    { 0x2E, KEY_W, "C" },
    { 0x2F, KEY_W, "V" },
    { 0x30, KEY_W, "B" },
    { 0x31, KEY_W, "N" },
    { 0x32, KEY_W, "M" },
    { 0x33, KEY_W, "," },
    { 0x34, KEY_W, "." },
    { 0x35, KEY_W, "/" },
    { 0x36, 44, "Shift" },
    { 0x1D, 25, "Ctrl" },
    { 0x38, 26, "Alt" },
    { 0x39, 127, "Space" },
    { 0xe038, 26, "Alt" },
    { 0xe01d, 25, "Ctrl" },
    { 0x52, KEY_W, "Ins" },
    { 0x47, KEY_W, "Hom" },
    { 0x49, KEY_W, "PgU" },
    { 0x53, KEY_W, "Del" },
    { 0x4F, KEY_W, "End" },
    { 0x51, KEY_W, "PgD" },
    { 0x48, KEY_W, "^" },
    { 0x4B, KEY_W, "<" },
    { 0x50, KEY_W, "v" },
    { 0x4D, KEY_W, ">" },
};

#define KEY_COUNT (sizeof(keys) / sizeof(keys[0]))

static window_st window;

static void
init_keys(void)
{
    int i;

    window.size.width = 0;
    window.size.height = 0;

    for (i = 0; i < KEY_COUNT; ++i) {
        keys[i].x = 0;
        keys[i].y = 0;
    }

    keys[1].x = 27;
    keys[5].x = 98;
    keys[9].x = 169;
    keys[13].y = KEY_H + 5;
    keys[27].y = KEY_H * 2 + 5;
    keys[41].y = KEY_H * 3 + 5;
    keys[54].y = KEY_H * 4 + 5;
    keys[66].y = KEY_H * 5 + 5;
    keys[71].x = 240;
    keys[71].y = KEY_H + 5;
    keys[74].x = 240;
    keys[74].y = KEY_H * 2 + 5;
    keys[77].x = 240 + KEY_W;
    keys[77].y = KEY_H * 4 + 5;
    keys[78].x = 240;
    keys[78].y = KEY_H * 5 + 5;

    for (i = 1; i < KEY_COUNT; ++i) {
        if (!keys[i].x && !keys[i].y) {
            keys[i].x = keys[i - 1].x + keys[i - 1].width;
            keys[i].y = keys[i - 1].y;
        }

        if (keys[i].x + keys[i].width > window.size.width) {
            window.size.width = keys[i].x + keys[i].width;
        }

        if (keys[i].y + KEY_H > window.size.height) {
            window.size.height = keys[i].y + KEY_H;
        }
    }
}

static void
draw_key(const key_st *key, int pressed)
{
    rect_st rect;
    uint8_t fg = pressed ? COLOR_BG : COLOR_FG;
    uint8_t bg = pressed ? COLOR_FG : COLOR_BG;

    gui_rect_init(&rect, key->x, key->y, key->width + 1, KEY_H + 1);
    gui_surface_draw_rect(&window.origin, &rect, bg);

    if (!pressed) {
        gui_surface_draw_border(&window.origin, &rect, COLOR_FG);
    }

    gui_surface_draw_str_centered(&window.origin, &rect, &fonts[1], key->label, fg, bg);
    gui_wm_render_window_region(&window.origin, &rect);
}

static void
draw_keyboard(void)
{
    int i;

    for (i = 0; i < KEY_COUNT; ++i) {
        draw_key(&keys[i], 0);
    }
}

static const key_st *
find_key(uint16_t code)
{
    int i;

    for (i = 0; i < KEY_COUNT; ++i) {
        if (keys[i].code == code) {
            return &keys[i];
        }
    }

    return NULL;
}

static void
update_key(uint16_t code, int escaped, int pressed)
{
    const key_st *key = NULL;

    if (escaped) {
        key = find_key(code | 0xe000);
    }

    if (!key) {
        key = find_key(code);
    }

    if (key) {
        draw_key(key, pressed);
    }
}

static void
on_key_down(window_st *win, const event_st *event)
{
    update_key(event->payload.key.key_code, event->payload.key.key_escaped, 1);
}

static void
on_key_up(window_st *win, const event_st *event)
{
    update_key(event->payload.key.key_code, event->payload.key.key_escaped, 0);
}

static void
init_window(void)
{
    gui_window_init(&window, window.size.width, window.size.height);
    window.bg_color = COLOR_BG;
    window.on_key_down = on_key_down;
    window.on_key_up = on_key_up;
}

static void
show_app(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_keys();
        init_window();
        initialized = 1;
    }

    gui_wm_add_window(&window);
    draw_keyboard();
}

app_st app_keymap = {
    "Keys",
    &bitmap_icon_keys,
    show_app,
};
