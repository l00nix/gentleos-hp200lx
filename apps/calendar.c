/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: calendar.c - Calendar app
 */

#include <gui.h>

enum {
    GRID_CELL_WIDTH = 32,
    GRID_CELL_HEIGHT = 16,

    TOOL_BAR_Y = 0,
    TOOL_BAR_HEIGHT = GRID_CELL_HEIGHT + 2,

    WEEK_BAR_Y = (TOOL_BAR_Y + TOOL_BAR_HEIGHT - 1),
    WEEK_BAR_HEIGHT = 16,

    GRID_ROWS = 6,
    GRID_COLS = 7,
    GRID_CELLS_COUNT = GRID_ROWS * GRID_COLS,
    GRID_WIDTH = GRID_WIDTH_SPACED(GRID_CELL_WIDTH, GRID_COLS),
    GRID_HEIGHT = GRID_HEIGHT_SPACED(GRID_CELL_HEIGHT, GRID_ROWS),
    GRID_X = 1,
    GRID_Y = WEEK_BAR_Y + WEEK_BAR_HEIGHT,

    WINDOW_WIDTH = GRID_X + GRID_WIDTH + 1,
    WINDOW_HEIGHT = GRID_Y + GRID_HEIGHT + 1,
};

static widget_st day_buttons[GRID_CELLS_COUNT];
static widget_st *widgets[GRID_CELLS_COUNT];

static window_st window;

enum {
    MIN_YEAR = 1900,
    MAX_YEAR = 2099,
};

static int current_month = 0;
static int current_year = 0;
static int current_day = 0;

static int selected_month = 0;
static int selected_year = 0;

static grid_st grid;

static int
get_day_of_week(int day, int month, int year)
{
    static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

    if (month < 3) {
        --year;
    }

    return (year + year / 4 - year / 100 + year / 400 + t[month - 1] + day) % 7;
}

static int
get_num_days_in_month(int month, int year)
{
    static int days_in_month[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int is_leap = (year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0));

    return (is_leap && month == 2) ? 29 : days_in_month[month];
}

static void
draw_month_label(void)
{
    static const char *month_names[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    char buf[16];
    rect_st rect;

    rect.x = 1;
    rect.y = TOOL_BAR_Y;
    rect.width = WINDOW_WIDTH - 2;
    rect.height = TOOL_BAR_HEIGHT;

    snprintf(buf, sizeof(buf), "%s %d", month_names[selected_month - 1], selected_year);

    gui_surface_draw_border(&window.origin, &rect, COLOR_FG);
    gui_surface_draw_str_centered(&window.origin, &rect, NULL, buf,
        COLOR_FG, COLOR_BG);
    gui_wm_render_window_region(&window.origin, &rect);
}

static void
draw_day_button(widget_st *widget)
{
    int day = widget->tag1;
    int num_days = get_num_days_in_month(selected_month, selected_year);
    int is_in_month = day >= 0 && day < num_days;
    int is_current = (day == current_day - 1 && selected_month == current_month
        && selected_year == current_year);
    int fg, bg;
    char buf[3];

    if (!is_in_month) {
        gui_surface_draw_rect(&widget->window->origin, &widget->rect, COLOR_BG);
        gui_wm_render_window_region(&widget->window->origin, &widget->rect);
        return;
    }

    fg = is_current ? COLOR_BG : COLOR_FG;
    bg = is_current ? COLOR_FG : COLOR_BG;

    gui_surface_draw_rect(&widget->window->origin, &widget->rect, bg);

    snprintf(buf, sizeof(buf), "%d", day + 1);
    gui_surface_draw_str_centered(
        &widget->window->origin,
        &widget->rect,
        widget->font,
        buf,
        fg,
        bg
    );

    gui_wm_render_window_region(&widget->window->origin, &widget->rect);
}

static void
draw_selected_month(void)
{
    int day_of_week = get_day_of_week(1, selected_month, selected_year);
    size_t i;

    for (i = 0; i < GRID_CELLS_COUNT; ++i) {
        day_buttons[i].tag1 = i - day_of_week;
        gui_widget_draw(&day_buttons[i]);
    }

    draw_month_label();
}

static void
draw_week_bar(void)
{
    static const char *day_names[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    int y;
    rect_st rect;

    for (y = 0; y < 7; y++) {
        rect.x = y * (GRID_CELL_WIDTH + 2) - y;
        rect.y = WEEK_BAR_Y;
        rect.width = GRID_CELL_WIDTH + 2;
        rect.height = WEEK_BAR_HEIGHT;

        gui_surface_draw_border(&window.origin, &rect, COLOR_FG);
        gui_surface_draw_str_centered(&window.origin, &rect, NULL,
            day_names[y], COLOR_FG, COLOR_BG);
    }
}

static void
set_prev_month(void)
{
    if (selected_month > 1) {
        selected_month -= 1;
    } else if (selected_year > MIN_YEAR) {
        selected_year -= 1;
        selected_month = 12;
    } else {
        return;
    }

    draw_selected_month();
}

static void
set_next_month(void)
{
    if (selected_month < 12) {
        selected_month += 1;
    } else if (selected_year < MAX_YEAR) {
        selected_year += 1;
        selected_month = 1;
    } else {
        return;
    }

    draw_selected_month();
}

static void
on_key_up(window_st *window _unsd, const event_st *event _unsd)
{
    int ch = event->payload.key.key_char;

    switch (ch) {
    case 'n': set_next_month(); break;
    case 'p': set_prev_month(); break;
    }
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.title = "Calendar";
    window.bg_color = COLOR_BG;
    window.widgets = widgets;
    window.widgets_capacity = sizeof(widgets) / sizeof(widgets[0]);
    window.on_key_up = on_key_up;
}

static void
init_day_buttons(void)
{
    uint16_t i;
    int col, row;

    grid.cell_width = GRID_CELL_WIDTH;
    grid.cell_height = GRID_CELL_HEIGHT;
    grid.cols = GRID_COLS;
    grid.rows = GRID_ROWS;
    grid.x = GRID_X;
    grid.y = GRID_Y;

    memset(day_buttons, 0, sizeof(day_buttons));

    for (i = 0; i < GRID_CELLS_COUNT; ++i) {
        col = i % GRID_COLS;
        row = i / GRID_COLS;

        day_buttons[i].type = WIDGET_TYPE_BUTTON;
        gui_grid_cell_rect(&grid, col, row, &day_buttons[i].rect);
        day_buttons[i].draw = draw_day_button;
        day_buttons[i].press_on_move_in = 1;

        gui_window_add_widget(&window, &day_buttons[i]);
    }
}

static void
init_current_date(void)
{
    time_st t;
    krn_rtc_get_time(&t);

    current_month = t.month;
    current_year = t.year;
    current_day = t.day;

    selected_month = current_month;
    selected_year = current_year;
}

static void
show_app(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_window();
        init_day_buttons();
        init_current_date();

        initialized = 1;
    }

    gui_wm_add_window(&window);
    gui_window_draw(&window);
    draw_week_bar();
    draw_selected_month();
    gui_status_set("p:prev month  n:next month");
}


app_st app_calendar = {
    &bitmap_icon_calendar,
    show_app,
};
