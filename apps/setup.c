/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: setup.c - Settings app
 */

#include <gui.h>

enum {
    FONT_WIDTH = 5,
    FONT_HEIGHT = 8,

    ROW_HEIGHT = FONT_HEIGHT + 5,
    ROW_COUNT = 3,

    PADDING_Y = 10,
    PADDING_X = 15,

    WINDOW_WIDTH = 160,
    WINDOW_HEIGHT = PADDING_Y + ROW_HEIGHT * ROW_COUNT + PADDING_Y,

    CONTENT_X = PADDING_X,
    CONTENT_Y = PADDING_Y,
};

enum {
    FIELD_YEAR,
    FIELD_MONTH,
    FIELD_DAY,
    FIELD_HOUR,
    FIELD_MINUTE,
    FIELD_SECOND,
    FIELD_COLORS,
};

static window_st window;
static int cursor = 0;

/*
 * Row 0: "Date:  YYYY-MM-DD"
 * Row 1: "Time:  HH:MM:SS"
 */

 static struct {
    uint8_t row;
    uint8_t col;
    uint8_t len;
    int min;
    int max;
    int val;
} fields[] = {
    { 0,  9,  4, 2000, 2099 },
    { 0, 14,  2,    1,   12 },
    { 0, 17,  2,    1,   31 },
    { 1,  9,  2,    0,   23 },
    { 1, 12,  2,    0,   59 },
    { 1, 15,  2,    0,   59 },
    { 2,  9, 10,    0,    1 },
};

static const struct {
    uint8_t row;
    uint8_t col;
    const char *text;
} labels[] = {
    { 0,  0, "Date:" },
    { 0, 13, "-" },
    { 0, 16, "-" },
    { 1,  0, "Time:" },
    { 1, 11, ":" },
    { 1, 14, ":" },
    { 2,  0, "Colors:" },
};

#define FIELD_COUNT (sizeof(fields) / sizeof(fields[0]))
#define LABEL_COUNT (sizeof(labels) / sizeof(labels[0]))

static void
draw_field(int n)
{
    rect_st rect;
    char buf[16];

    int val = fields[n].val;
    int is_current = (n == cursor);

    uint8_t fg = is_current ? gui_color_bg : gui_color_fg;
    uint8_t bg = is_current ? gui_color_fg : gui_color_bg;

    rect.x = CONTENT_X + fields[n].col * FONT_WIDTH;
    rect.y = CONTENT_Y + fields[n].row * ROW_HEIGHT + (ROW_HEIGHT - FONT_HEIGHT) / 2 - 1;
    rect.width = fields[n].len * FONT_WIDTH;
    rect.height = FONT_HEIGHT + 1;

    if (n == FIELD_COLORS) {
        snprintf(buf, sizeof(buf), val == 1 ? "inverted" : "normal");
    } else if (n == FIELD_YEAR) {
        snprintf(buf, sizeof(buf), "%04d", val);
    } else {
        snprintf(buf, sizeof(buf), "%02d", val);
    }

    gui_surface_draw_rect(&window.origin, &rect, bg);
    gui_surface_draw_str(&window.origin, rect.x, rect.y + 1, NULL, buf, fg, bg);
    gui_surface_mark_dirty(&window.origin, &rect);
}

static void
draw_label(int n)
{
    int x, y;

    x = CONTENT_X + labels[n].col * FONT_WIDTH;
    y = CONTENT_Y + labels[n].row * ROW_HEIGHT + (ROW_HEIGHT - FONT_HEIGHT) / 2;

    gui_surface_draw_str(&window.origin, x, y, NULL, labels[n].text, gui_color_fg, gui_color_bg);
}

static void
draw_all(void)
{
    int i;
    rect_st r;

    gui_window_draw(&window, gui_color_bg, 1);

    for (i = 0; i < LABEL_COUNT; ++i) {
        draw_label(i);
    }

    for (i = 0; i < FIELD_COUNT; ++i) {
        draw_field(i);
    }

    gui_rect_init(&r, 0, 0, window.size.width, window.size.height);
    gui_surface_mark_dirty(&window.origin, &r);
}

static void
move_cursor(int dn)
{
    int prev_cursor = cursor;

    cursor = cursor + dn;
    cursor = MIN(cursor, FIELD_COUNT - 1);
    cursor = MAX(cursor, 0);

    if (prev_cursor == cursor) {
        return;
    }

    draw_field(prev_cursor);
    draw_field(cursor);
}

static void
edit_field(int dv)
{
    int min_val = fields[cursor].min;
    int max_val = fields[cursor].max;
    int prev_val = fields[cursor].val;
    int new_val = MAX(min_val, MIN(max_val, prev_val + dv));

    if (prev_val != new_val) {
        fields[cursor].val = new_val;
        draw_field(cursor);
    }
}

static void
load_fields(void)
{
    time_st time;
    date_st date;

    bios_get_time(&time);
    bios_get_date(&date);

    fields[FIELD_YEAR].val = date.year;
    fields[FIELD_MONTH].val = date.month;
    fields[FIELD_DAY].val = date.day;
    fields[FIELD_HOUR].val = time.hour;
    fields[FIELD_MINUTE].val = time.minute;
    fields[FIELD_SECOND].val = time.second;
    fields[FIELD_COLORS].val = gui_colors_inverted;
}

static void
save_fields(void)
{
    bios_set_date(
        fields[FIELD_YEAR].val,
        fields[FIELD_MONTH].val,
        fields[FIELD_DAY].val
    );

    bios_set_time(
        fields[FIELD_HOUR].val,
        fields[FIELD_MINUTE].val,
        fields[FIELD_SECOND].val
    );

    gui_set_colors_inverted(fields[FIELD_COLORS].val);
    gui_status_set("Settings saved");
}

static void
on_key_down(const event_st *event)
{
    int key_code = event->payload.key.key_code;

    switch (key_code) {
    case KEY_UP: move_cursor(-1); return;
    case KEY_DOWN: move_cursor(1); return;
    case KEY_PGUP: edit_field(1); return;
    case KEY_PGDN: edit_field(-1); return;
    case KEY_S: save_fields(); return;
    }
}

static void
on_show(void)
{
    static int initialized = 0;

    if (!initialized) {
        gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

        app_setup.on_key_down = on_key_down;

        initialized = 1;
    }

    cursor = 0;
    load_fields();
    draw_all();

    gui_status_set("\x18\x19: Select field  PgUp/PgDn: Edit");
    gui_status_set_br("S: Save");
}

global app_st app_setup = {
    "Setup",
    &icon_setup,
    on_show,
};
