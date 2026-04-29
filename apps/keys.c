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
    int pressed;
} cell_st;

enum {
    CELL_H = 15,
    CELL_W = 15,
};

static cell_st cells[] = {
    { 0x01, CELL_W, "Esc" },
    { 0x3B, CELL_W, "F1" },
    { 0x3C, CELL_W, "F2" },
    { 0x3D, CELL_W, "F3" },
    { 0x3E, CELL_W, "F4" },
    { 0x3F, CELL_W, "F5" },
    { 0x40, CELL_W, "F6" },
    { 0x41, CELL_W, "F7" },
    { 0x42, CELL_W, "F8" },
    { 0x43, CELL_W, "F9" },
    { 0x44, CELL_W, "F10" },
    { 0x57, CELL_W, "F11" },
    { 0x58, CELL_W, "F12" },
    { 0x29, CELL_W, "`" },
    { 0x02, CELL_W, "1" },
    { 0x03, CELL_W, "2" },
    { 0x04, CELL_W, "3" },
    { 0x05, CELL_W, "4" },
    { 0x06, CELL_W, "5" },
    { 0x07, CELL_W, "6" },
    { 0x08, CELL_W, "7" },
    { 0x09, CELL_W, "8" },
    { 0x0A, CELL_W, "9" },
    { 0x0B, CELL_W, "0" },
    { 0x0C, CELL_W, "-" },
    { 0x0D, CELL_W, "=" },
    { 0x0E, 34, "Bksp" },
    { 0x0F, 23, "Tab" },
    { 0x10, CELL_W, "Q" },
    { 0x11, CELL_W, "W" },
    { 0x12, CELL_W, "E" },
    { 0x13, CELL_W, "R" },
    { 0x14, CELL_W, "T" },
    { 0x15, CELL_W, "Y" },
    { 0x16, CELL_W, "U" },
    { 0x17, CELL_W, "I" },
    { 0x18, CELL_W, "O" },
    { 0x19, CELL_W, "P" },
    { 0x1A, CELL_W, "[" },
    { 0x1B, CELL_W, "]" },
    { 0x2B, 26, "\\" },
    { 0x3A, 27, "Caps" },
    { 0x1E, CELL_W, "A" },
    { 0x1F, CELL_W, "S" },
    { 0x20, CELL_W, "D" },
    { 0x21, CELL_W, "F" },
    { 0x22, CELL_W, "G" },
    { 0x23, CELL_W, "H" },
    { 0x24, CELL_W, "J" },
    { 0x25, CELL_W, "K" },
    { 0x26, CELL_W, "L" },
    { 0x27, CELL_W, ";" },
    { 0x28, CELL_W, "'" },
    { 0x1C, 37, "Enter" },
    { 0x2A, 35, "Shift" },
    { 0x2C, CELL_W, "Z" },
    { 0x2D, CELL_W, "X" },
    { 0x2E, CELL_W, "C" },
    { 0x2F, CELL_W, "V" },
    { 0x30, CELL_W, "B" },
    { 0x31, CELL_W, "N" },
    { 0x32, CELL_W, "M" },
    { 0x33, CELL_W, "," },
    { 0x34, CELL_W, "." },
    { 0x35, CELL_W, "/" },
    { 0x36, 44, "Shift" },
    { 0x1D, 25, "Ctrl" },
    { 0x38, 26, "Alt" },
    { 0x39, 127, "Space" },
    { 0xe038, 26, "Alt" },
    { 0xe01d, 25, "Ctrl" },
    { 0x52, CELL_W, "Ins" },
    { 0x47, CELL_W, "Hom" },
    { 0x49, CELL_W, "PgU" },
    { 0x53, CELL_W, "Del" },
    { 0x4F, CELL_W, "End" },
    { 0x51, CELL_W, "PgD" },
    { 0x48, CELL_W, "^" },
    { 0x4B, CELL_W, "<" },
    { 0x50, CELL_W, "v" },
    { 0x4D, CELL_W, ">" },
};

#define CELL_COUNT (sizeof(cells) / sizeof(cells[0]))

static window_st window;
static int last_key_code = 0;

static void
init_cells(void)
{
    int i;

    window.size.width = 0;
    window.size.height = 0;

    for (i = 0; i < CELL_COUNT; ++i) {
        cells[i].x = 0;
        cells[i].y = 0;
        cells[i].pressed = 0;
    }

    cells[1].x = 27;
    cells[5].x = 98;
    cells[9].x = 169;
    cells[13].y = CELL_H + 5;
    cells[27].y = CELL_H * 2 + 5;
    cells[41].y = CELL_H * 3 + 5;
    cells[54].y = CELL_H * 4 + 5;
    cells[66].y = CELL_H * 5 + 5;
    cells[71].x = 240;
    cells[71].y = CELL_H + 5;
    cells[74].x = 240;
    cells[74].y = CELL_H * 2 + 5;
    cells[77].x = 240 + CELL_W;
    cells[77].y = CELL_H * 4 + 5;
    cells[78].x = 240;
    cells[78].y = CELL_H * 5 + 5;

    for (i = 1; i < CELL_COUNT; ++i) {
        if (!cells[i].x && !cells[i].y) {
            cells[i].x = cells[i - 1].x + cells[i - 1].width;
            cells[i].y = cells[i - 1].y;
        }

        if (cells[i].x + cells[i].width > window.size.width) {
            window.size.width = cells[i].x + cells[i].width;
        }

        if (cells[i].y + CELL_H > window.size.height) {
            window.size.height = cells[i].y + CELL_H;
        }
    }
}

static void
draw_cell(cell_st *key, int pressed)
{
    rect_st rect;
    uint8_t fg = pressed ? gui_color_bg : gui_color_fg;
    uint8_t bg = pressed ? gui_color_fg : gui_color_bg;

    gui_rect_init(&rect, key->x, key->y, key->width + 1, CELL_H + 1);
    gui_surface_draw_rect(&window.origin, &rect, bg);

    if (!pressed) {
        gui_surface_draw_border(&window.origin, &rect, gui_color_fg);
    }

    gui_surface_draw_str_centered(&window.origin, &rect, &fonts[1], key->label, fg, bg);
    gui_surface_mark_dirty(&window.origin, &rect);
}

static void
draw_keyboard(void)
{
    int i;

    for (i = 0; i < CELL_COUNT; ++i) {
        draw_cell(&cells[i], 0);
    }
}

static cell_st *
find_key(uint16_t code)
{
    int i;

    for (i = 0; i < CELL_COUNT; ++i) {
        if (cells[i].code == code) {
            return &cells[i];
        }
    }

    return NULL;
}

static void
update_cell(uint16_t code, int escaped, int pressed)
{
    cell_st *key = NULL;

    if (escaped) {
        key = find_key(code | 0xe000);
    }

    if (!key) {
        key = find_key(code);
    }

    if (key && key->pressed != pressed) {
        draw_cell(key, pressed);
        key->pressed = pressed;
    }
}

static void
on_key_down(uint8_t key_code, uint8_t key_mods)
{
    int escaped = !!(key_mods & KEY_MOD_ESC);
    char key_char = key_char_for_code(key_code, key_mods);

    update_cell(key_code, escaped, 1);

    if (key_code != last_key_code) {
        gui_status_set("Last key:%02X  Mods:%02X  Char:%02X (%c)",
            key_code, key_mods, key_char, key_char ? key_char : ' ');
    }
}

static void
on_key_up(uint8_t key_code, uint8_t key_mods)
{
    int escaped = !!(key_mods & KEY_MOD_ESC);

    update_cell(key_code, escaped, 0);
}

static void
on_show(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_cells();

        gui_window_init(&window, window.size.width, window.size.height);

        app_keymap.on_key_down = on_key_down;
        app_keymap.on_key_up = on_key_up;

        initialized = 1;
    }

    draw_keyboard();
}

global app_st app_keymap = {
    "Keys",
    &icon_keys,
    on_show,
};
