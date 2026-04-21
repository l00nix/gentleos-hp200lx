/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: sounds.c - Sound playing app
 */

#include <gui.h>

enum {
    KEY_W_WIDTH = 15,
    KEY_W_HEIGHT = 80,
    KEY_W_COUNT = 15,

    KEY_B_WIDTH = 9,
    KEY_B_HEIGHT = 50,
    KEY_B_COUNT = 10,

    KEYBOARD_Y = 0,
    KEYBOARD_HEIGHT = (KEY_W_HEIGHT),

    WINDOW_WIDTH = ((KEY_W_COUNT * KEY_W_WIDTH) - (KEY_W_COUNT - 1)),
    WINDOW_HEIGHT = (KEYBOARD_Y + KEYBOARD_HEIGHT),

    TAG_KEY_W = 1,
    TAG_KEY_B = 2,
};

static window_st window;

static widget_st keys_w[KEY_W_COUNT];
static widget_st keys_b[KEY_B_COUNT];
static widget_st *widgets[KEY_W_COUNT + KEY_B_COUNT];
static widget_st *pressed_widget = 0;

static void
draw_key_w(widget_st *widget)
{
    rect_st rect_base;
    uint8_t color = (widget == pressed_widget) ? COLOR_FG : COLOR_BG;

    int octave = widget->tag2 / 7;
    int ofs = widget->tag2 % 7;

    rect_st rect_top;
    rect_st rect_bottom;

    gui_rect_copy(&rect_base, &widget->rect);
    gui_rect_shrink(&rect_base, 1);

    gui_rect_copy(&rect_top, &rect_base);
    if (ofs == 1 || ofs == 2 || ofs == 4 || ofs == 5 || ofs == 6) {
        rect_top.x += KEY_B_WIDTH / 2;
        rect_top.width -= KEY_B_WIDTH / 2;
    }
    if ((ofs == 0 && octave < 2) || ofs == 1 || ofs == 3 || ofs == 4 || ofs == 5) {
        rect_top.width -= KEY_B_WIDTH / 2;
    }
    gui_surface_draw_rect(&widget->window->origin, &rect_top, color);

    gui_rect_copy(&rect_bottom, &rect_base);
    rect_bottom.y += KEY_B_HEIGHT;
    rect_bottom.height -= KEY_B_HEIGHT;
    gui_surface_draw_rect(&widget->window->origin, &rect_bottom, color);

    gui_surface_mark_dirty(&widget->window->origin, &widget->rect);
}

static void
draw_key_b(widget_st *widget)
{
    uint8_t color = (widget == pressed_widget) ? COLOR_BG : COLOR_FG;

    gui_surface_draw_rect(&widget->window->origin, &widget->rect, color);

    gui_surface_mark_dirty(&widget->window->origin, &widget->rect);
}

static unsigned
key_frequency(widget_st *widget)
{
    static unsigned freqs_w[] = { 131, 147, 165, 175, 196, 220, 247 };
    static unsigned freqs_b[] = { 139, 156, 185, 208, 233 };

    int is_w = widget->tag1 == TAG_KEY_W;
    unsigned *freqs = is_w ? freqs_w : freqs_b;
    unsigned octave = is_w ? widget->tag2 / 7 : widget->tag2 / 5;
    unsigned ofs = is_w ? widget->tag2 % 7 : widget->tag2 % 5;

    return freqs[ofs] * (1 << octave);
}

static widget_st *
key_for_char(int ch)
{
    widget_st *w = NULL;

    if (ch >= 'A' && ch <= 'Z') {
        ch += 32;
    }

    switch (ch) {
    case 'z': w = &keys_w[0]; break;
    case 'x': w = &keys_w[1]; break;
    case 'c': w = &keys_w[2]; break;
    case 'v': w = &keys_w[3]; break;
    case 'b': w = &keys_w[4]; break;
    case 'n': w = &keys_w[5]; break;
    case 'm': w = &keys_w[6]; break;
    case ',': w = &keys_w[7]; break;
    case 'w': w = &keys_w[7]; break;
    case 'e': w = &keys_w[8]; break;
    case 'r': w = &keys_w[9]; break;
    case 't': w = &keys_w[10]; break;
    case 'y': w = &keys_w[11]; break;
    case 'u': w = &keys_w[12]; break;
    case 'i': w = &keys_w[13]; break;
    case 'o': w = &keys_w[14]; break;
    case 's': w = &keys_b[0]; break;
    case 'd': w = &keys_b[1]; break;
    case 'g': w = &keys_b[2]; break;
    case 'h': w = &keys_b[3]; break;
    case 'j': w = &keys_b[4]; break;
    case '#':
    case '3': w = &keys_b[5]; break;
    case '$':
    case '4': w = &keys_b[6]; break;
    case '^':
    case '6': w = &keys_b[7]; break;
    case '&':
    case '7': w = &keys_b[8]; break;
    case '*':
    case '8': w = &keys_b[9]; break;
    }

    return w;
}

static void
on_key_down(window_st *window, const event_st *event)
{
    int ch = event->payload.key.key_char;
    widget_st *widget = key_for_char(ch);
    widget_st *prev_widget;

    if (!widget) {
        return;
    }

    if (pressed_widget) {
        prev_widget = pressed_widget;
        pressed_widget = NULL;
        gui_widget_draw(prev_widget);
    }

    krn_speaker_play(key_frequency(widget));
    pressed_widget = widget;
    gui_widget_draw(widget);
}

static void
on_key_up(window_st *window, const event_st *event)
{
    int ch = event->payload.key.key_char;
    widget_st *widget = key_for_char(ch);

    if (!widget || widget != pressed_widget) {
        return;
    }

    pressed_widget = NULL;
    gui_widget_draw(widget);
    krn_speaker_stop();
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.bg_color = COLOR_FG;
    window.widgets = widgets;
    window.widgets_capacity = sizeof(widgets) / sizeof(widgets[0]);
    window.on_key_down = on_key_down;
    window.on_key_up = on_key_up;
}

static void
init_keys(void)
{
    int i;
    int octave_no, octave_ofs, key_w_idx;

    memset(keys_b, 0, sizeof(keys_b));
    memset(keys_w, 0, sizeof(keys_w));

    for (i = 0; i < KEY_B_COUNT; i++) {
        octave_no = i / 5;
        octave_ofs = i % 5;
        key_w_idx = (octave_no * 7) + octave_ofs + 1 + (octave_ofs > 1 ? 1 : 0);

        keys_b[i].type = WIDGET_TYPE_BUTTON;
        keys_b[i].rect.x = (key_w_idx * KEY_W_WIDTH) - key_w_idx - (KEY_B_WIDTH / 2);
        keys_b[i].rect.y = 1;
        keys_b[i].rect.width = KEY_B_WIDTH;
        keys_b[i].rect.height = KEY_B_HEIGHT;
        keys_b[i].draw = draw_key_b;
        keys_b[i].tag1 = TAG_KEY_B;
        keys_b[i].tag2 = i;
        gui_window_add_widget(&window, &keys_b[i]);
    }

    for (i = 0; i < KEY_W_COUNT; i++) {
        keys_w[i].type = WIDGET_TYPE_BUTTON;
        keys_w[i].rect.x = (i * KEY_W_WIDTH) - i;
        keys_w[i].rect.y = 0;
        keys_w[i].rect.width = KEY_W_WIDTH;
        keys_w[i].rect.height = KEY_W_HEIGHT;
        keys_w[i].draw = draw_key_w;
        keys_w[i].tag1 = TAG_KEY_W;
        keys_w[i].tag2 = i;
        gui_window_add_widget(&window, &keys_w[i]);
    }
}

static void
on_show(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_window();
        init_keys();
        initialized = 1;
    }

    gui_window_draw(&window);

    gui_status_set("Control: letters and digits");
}

global app_st app_sounds = {
    "Sounds",
    &icon_sounds,
    &window,
    on_show,
};
