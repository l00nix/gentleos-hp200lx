/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: pairs.c - Pair matching / Memory game
 */

#include <gui.h>

enum {
    GRID_CELL_WIDTH = 36,
    GRID_CELL_HEIGHT = 36,
    GRID_ROWS = 4,
    GRID_COLS = 5,
    GRID_CELL_COUNT = GRID_ROWS * GRID_COLS,
    GRID_WIDTH = GRID_WIDTH_SPACED(GRID_CELL_WIDTH, GRID_COLS),
    GRID_HEIGHT = GRID_HEIGHT_SPACED(GRID_CELL_HEIGHT, GRID_ROWS),
    GRID_X = 1,
    GRID_Y = 1,

    WINDOW_WIDTH = GRID_X + GRID_WIDTH + 1,
    WINDOW_HEIGHT = GRID_Y + GRID_HEIGHT + 1,

    PAIR_COUNT = GRID_CELL_COUNT / 2,
    MISMATCH_DELAY = 800,
};

static window_st window;

static widget_st buttons[GRID_CELL_COUNT];
static widget_st *widgets[GRID_CELL_COUNT];

static grid_st grid;

static bitmap_st *icons[PAIR_COUNT] = {
    &bitmap_icon_pairs_bear,
    &bitmap_icon_pairs_bot,
    &bitmap_icon_pairs_butterfly,
    &bitmap_icon_pairs_cat,
    &bitmap_icon_pairs_elephant,
    &bitmap_icon_pairs_fish,
    &bitmap_icon_pairs_ghost,
    &bitmap_icon_pairs_rabbit,
    &bitmap_icon_pairs_snake,
    &bitmap_icon_pairs_turtle,
};

enum {
    BUTTON_STATE_HIDDEN = 0,
    BUTTON_STATE_REVEALED = 1,
    BUTTON_STATE_MATCHED = 2,
};

static uint8_t button_icons[GRID_CELL_COUNT];
static uint8_t button_states[GRID_CELL_COUNT];

static int first_pick;
static int second_pick;
static int tries;
static int matched_count;
static int waiting;

static void
shuffle_icons(void)
{
    uint8_t deck[GRID_CELL_COUNT];
    int i, j;
    uint8_t tmp;

    for (i = 0; i < PAIR_COUNT; i++) {
        deck[i * 2] = i;
        deck[i * 2 + 1] = i;
    }

    for (i = GRID_CELL_COUNT - 1; i > 0; i--) {
        j = rand() % (i + 1);
        tmp = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp;
    }

    for (i = 0; i < GRID_CELL_COUNT; i++) {
        button_icons[i] = deck[i];
    }
}

static void
draw_button(widget_st *widget)
{
    int idx = widget->tag1;
    uint8_t state = button_states[idx];
    rect_st rect;

    gui_rect_copy(&rect, &widget->rect);
    gui_surface_draw_rect(&window.origin, &rect, COLOR_BG);

    if (state == BUTTON_STATE_REVEALED || state == BUTTON_STATE_MATCHED) {
        gui_surface_draw_bitmap_centered(&window.origin, &window.size, &rect,
            icons[button_icons[idx]], COLOR_FG);
    }

    if (widget == widget->window->focused_widget) {
        gui_surface_draw_border(&window.origin, &rect, COLOR_FG);
    }

    gui_wm_render_window_region(&window.origin, &rect);
}

static void
reveal_icon(int idx)
{
    button_states[idx] = BUTTON_STATE_REVEALED;
    draw_button(&buttons[idx]);
}

static void
hide_icon(int idx)
{
    button_states[idx] = BUTTON_STATE_HIDDEN;
    draw_button(&buttons[idx]);
}

static void
update_status(void)
{
    if (matched_count == PAIR_COUNT) {
        gui_status_set("You won after %d tries! Press r play again", tries);
    } else {
        gui_status_set("Tries: %d", tries);
    }
}

static void
restart_game(void)
{
    int i;

    shuffle_icons();

    first_pick = -1;
    second_pick = -1;
    tries = 0;
    matched_count = 0;
    waiting = 0;

    for (i = 0; i < GRID_CELL_COUNT; i++) {
        button_states[i] = BUTTON_STATE_HIDDEN;
        draw_button(&buttons[i]);
    }

    update_status();
}

static void
on_tick(window_st *window)
{
    if (!waiting) {
        return;
    }

    if (--waiting) {
        return;
    }

    hide_icon(first_pick);
    first_pick = -1;

    hide_icon(second_pick);
    second_pick = -1;

    waiting = 0;

    update_status();
}

static void
on_cell_press(widget_st *widget)
{
    int idx = widget->tag1;

    if (waiting) {
        return;
    }

    if (button_states[idx] != BUTTON_STATE_HIDDEN) {
        return;
    }

    if (first_pick == -1) {
        first_pick = idx;
        reveal_icon(first_pick);
        return;
    }

    second_pick = idx;
    reveal_icon(second_pick);
    tries++;

    if (button_icons[first_pick] == button_icons[second_pick]) {
        button_states[first_pick] = BUTTON_STATE_MATCHED;
        button_states[second_pick] = BUTTON_STATE_MATCHED;
        first_pick = -1;
        second_pick = -1;
        matched_count++;
    } else {
        waiting = 20;
    }

    update_status();
}

static void
on_key_up(window_st *window _unsd, const event_st *event)
{
    int ch = event->payload.key.key_char;

    if (ch == 'r' && matched_count == PAIR_COUNT) {
        restart_game();
        return;
    }
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.bg_color = COLOR_FG;
    window.widgets = widgets;
    window.widgets_capacity = sizeof(widgets) / sizeof(widgets[0]);
    window.focused_widget = &buttons[0];
    window.on_tick = on_tick;
    window.on_key_up = on_key_up;
}

static void
init_grid(void)
{
    int i, col, row;

    grid.cell_width = GRID_CELL_WIDTH;
    grid.cell_height = GRID_CELL_HEIGHT;
    grid.cols = GRID_COLS;
    grid.rows = GRID_ROWS;
    grid.x = GRID_X;
    grid.y = GRID_Y;

    memset(buttons, 0, sizeof(buttons));

    for (i = 0; i < GRID_CELL_COUNT; i++) {
        col = i % GRID_COLS;
        row = i / GRID_COLS;

        buttons[i].type = WIDGET_TYPE_BUTTON;
        gui_grid_cell_rect(&grid, col, row, &buttons[i].rect);
        buttons[i].tag1 = i;
        buttons[i].draw = draw_button;
        buttons[i].on_press = on_cell_press;
        buttons[i].hide_border = 1;
        buttons[i].focusable = 1;
        buttons[i].focus_x = col;
        buttons[i].focus_y = row;

        gui_window_add_widget(&window, &buttons[i]);
    }
}

static void
show_app(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_window();
        init_grid();
        initialized = 1;
    }

    gui_wm_add_window(&window);
    gui_window_draw(&window);
    restart_game();
}

app_st app_pairs = {
    "Pairs",
    &bitmap_icon_pairs,
    show_app,
};
