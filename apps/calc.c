/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: calc.c - Calculator app
 */

#include <gui.h>

enum {
    BUTTON_WIDTH = 36,
    BUTTON_HEIGHT = 36,
    BUTTON_COLS = 4,
    BUTTON_ROWS = 4,
    BUTTONS_COUNT = BUTTON_COLS * BUTTON_ROWS,

    DISPLAY_HEIGHT = BUTTON_HEIGHT,

    GRID_X = 1,
    GRID_Y = 1 + DISPLAY_HEIGHT + 1,
    GRID_WIDTH = GRID_WIDTH_SPACED(BUTTON_WIDTH, BUTTON_COLS),
    GRID_HEIGHT = GRID_HEIGHT_SPACED(BUTTON_HEIGHT, BUTTON_ROWS),

    WINDOW_WIDTH = GRID_X + GRID_WIDTH + 1,
    WINDOW_HEIGHT = GRID_Y + GRID_HEIGHT + 1,

    DISPLAY_X = GRID_X,
    DISPLAY_Y = 1,
    DISPLAY_WIDTH = GRID_WIDTH,
};

static window_st window;

static widget_st button_widgets[BUTTONS_COUNT];
static widget_st *widgets[BUTTONS_COUNT];

static grid_st grid;

static const char *button_labels[BUTTONS_COUNT] = {
    "7", "8", "9", "/",
    "4", "5", "6", "*",
    "1", "2", "3", "-",
    "0", "C", "=", "+"
};

typedef int32_t val_t;

#define VAL_MAX INT32_MAX
#define VAL_MIN INT32_MIN

static val_t current_val = 0;
static val_t stored_val = 0;
static val_t last_operand = 0;
static uint8_t current_op = 0;
static uint8_t last_op = 0;
static int new_number = 1;
static int error = 0;

static void
exec_add(void)
{
    val_t result;

    if (__builtin_add_overflow(stored_val, current_val, &result)) {
        error = 1;
        return;
    }

    current_val = result;
}

static void
exec_sub(void)
{
    val_t result;

    if (__builtin_sub_overflow(stored_val, current_val, &result)) {
        error = 1;
        return;
    }

    current_val = result;
}

static void
exec_mul(void)
{
    val_t result;

    if (__builtin_mul_overflow(current_val, stored_val, &result)) {
        error = 1;
        return;
    }

    current_val = result;
}

static void
exec_div(void)
{
    if (current_val == 0 || (stored_val == VAL_MIN && current_val == -1)) {
        error = 1;
        return;
    }

    current_val = stored_val / current_val;
}

static void
exec_current_op(void)
{
    switch (current_op) {
    case '+': exec_add(); break;
    case '-': exec_sub(); break;
    case '*': exec_mul(); break;
    case '/': exec_div(); break;
    }

    current_op = 0;
    new_number = 1;
}

static void
update_display(void)
{
    static char buf[32];
    rect_st rect;
    font_st *font;
    int text_width, text_x, text_y;

    if (error) {
        snprintf(buf, sizeof(buf), "ERR");
    } else {
        snprintf(buf, sizeof(buf), "%d", current_val);
    }

    gui_rect_init(&rect, DISPLAY_X, DISPLAY_Y, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    gui_surface_draw_rect(&window.origin, &rect, COLOR_BG);

    font = &fonts[0];
    text_width = strlen(buf) * font->size.width;
    text_x = rect.x + rect.width - text_width - 10;
    text_y = rect.y + (rect.height - font_8x8->size.height) / 2;
    gui_surface_draw_str(&window.origin, text_x, text_y, font,
        buf, COLOR_FG, COLOR_BG);

    gui_wm_render_window_region(&window.origin, &rect);
}

static void
on_button_press(widget_st *widget, const event_st *event _unsd, const point_st *pos _unsd)
{
    uint8_t op;
    int val;
    val_t new_val;

    gui_widget_draw(widget);

    op = widget->label[0];

    if (error && op != 'C') {
        return;
    }

    if (op >= '0' && op <= '9') {
        val = op - '0';
        if (new_number) {
            current_val = val;
            new_number = 0;
        } else {
            if (__builtin_mul_overflow(current_val, 10, &new_val) ||
                __builtin_add_overflow(new_val, val, &new_val)) {
                return;
            }
            current_val = new_val;
        }
    } else if (op == 'C') {
        current_val = 0;
        stored_val = 0;
        current_op = 0;
        last_op = 0;
        last_operand = 0;
        new_number = 1;
        error = 0;
    } else if (op == '=') {
        if (current_op) {
            last_op = current_op;
            last_operand = current_val;
            exec_current_op();
        } else if (last_op) {
            stored_val = current_val;
            current_val = last_operand;
            current_op = last_op;
            exec_current_op();
        }
    } else if (op == '+' || op == '-' || op == '*' || op == '/') {
        if (current_op && !new_number) {
            exec_current_op();
        }
        stored_val = current_val;
        current_op = op;
        last_op = 0;
        new_number = 1;
    }

    update_display();
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.title = "Calculator";
    window.bg_color = COLOR_FG;
    window.widgets = widgets;
    window.widgets_capacity = sizeof(widgets) / sizeof(widgets[0]);
}

static void
init_buttons(void)
{
    int row, col, idx;
    widget_st *button;

    grid.cell_width = BUTTON_WIDTH;
    grid.cell_height = BUTTON_HEIGHT;
    grid.cols = BUTTON_COLS;
    grid.rows = BUTTON_ROWS;
    grid.x = GRID_X;
    grid.y = GRID_Y;

    for (row = 0; row < BUTTON_ROWS; ++row) {
        for (col = 0; col < BUTTON_COLS; ++col) {
            idx = row * BUTTON_COLS + col;
            button = &button_widgets[idx];

            button->type = WIDGET_TYPE_BUTTON;
            gui_grid_cell_rect(&grid, col, row, &button->rect);
            button->hide_border = 1;
            button->window = &window;
            button->label = button_labels[idx];
            button->on_pointer_up = on_button_press;

            gui_window_add_widget(&window, button);
        }
    }
}

static void
show_app(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_window();
        init_buttons();
        initialized = 1;
    }

    gui_wm_add_window(&window);
    gui_window_draw(&window);
    update_display();
}

app_st app_calc = {
    "Calculator",
    &bitmap_icon_calc,
    show_app,
};
