/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: pairs.c - Pair matching / Memory game
 */

#include <gui.h>

enum {
    GRID_CELL_WIDTH = 26,
    GRID_CELL_HEIGHT = 26,
    GRID_ROWS = 5,
    GRID_COLS = 6,
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
static grid_st grid;

static bitmap_st *icons[PAIR_COUNT] = {
    &glyph_mn_beaver,
    &glyph_mn_dolphin,
    &glyph_mn_drmcamel,
    &glyph_mn_elephant,
    &glyph_mn_flamingo,
    &glyph_mn_horsefac,
    &glyph_mn_monkey,
    &glyph_mn_octopus,
    &glyph_mn_pandafac,
    &glyph_mn_rabbit,
    &glyph_mn_robotfac,
    &glyph_mn_sloth,
    &glyph_mn_snail,
    &glyph_mn_tigerfac,
    &glyph_mn_trex,
};

enum {
    CELL_STATE_HIDDEN = 0,
    CELL_STATE_REVEALED = 1,
    CELL_STATE_MATCHED = 2,
};

static uint8_t cell_icons[GRID_CELL_COUNT];
static uint8_t cell_states[GRID_CELL_COUNT];

static int current_col = 0;
static int current_row = 0;

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
        cell_icons[i] = deck[i];
    }
}

static void
draw_cell(int col, int row)
{
    int idx = row * GRID_COLS + col;
    uint8_t state = cell_states[idx];
    rect_st rect;

    gui_grid_cell_rect(&grid, col, row, &rect);
    gui_surface_draw_rect(&window.origin, &rect, COLOR_BG);

    if (state == CELL_STATE_REVEALED || state == CELL_STATE_MATCHED) {
        gui_surface_draw_bitmap_centered(&window.origin, &window.size, &rect,
            icons[cell_icons[idx]], COLOR_FG);
    }

    if (col == current_col && row == current_row) {
        gui_surface_draw_border(&window.origin, &rect, COLOR_FG);
    }

    gui_surface_mark_dirty(&window.origin, &rect);
}

static void
draw_cell_by_idx(int idx)
{
    draw_cell(idx % GRID_COLS, idx / GRID_COLS);
}

static void
reveal_icon(int idx)
{
    cell_states[idx] = CELL_STATE_REVEALED;
    draw_cell_by_idx(idx);
}

static void
hide_icon(int idx)
{
    cell_states[idx] = CELL_STATE_HIDDEN;
    draw_cell_by_idx(idx);
}

static void
update_status(void)
{
    if (matched_count == PAIR_COUNT) {
        gui_status_set("You won after %d tries! Press R to play again", tries);
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
        cell_states[i] = CELL_STATE_HIDDEN;
        draw_cell_by_idx(i);
    }

    update_status();
}

static void
on_tick(void)
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
update_current_cell(int dx, int dy)
{
    int prev_col = current_col;
    int prev_row = current_row;

    current_col = MAX(0, MIN(GRID_COLS - 1, current_col + dx));
    current_row = MAX(0, MIN(GRID_ROWS - 1, current_row + dy));

    draw_cell(prev_col, prev_row);
    draw_cell(current_col, current_row);
}

static void
on_enter(void)
{
    int idx = current_row * GRID_COLS + current_col;

    if (waiting) {
        return;
    }

    if (cell_states[idx] != CELL_STATE_HIDDEN) {
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

    if (cell_icons[first_pick] == cell_icons[second_pick]) {
        cell_states[first_pick] = CELL_STATE_MATCHED;
        cell_states[second_pick] = CELL_STATE_MATCHED;
        first_pick = -1;
        second_pick = -1;
        matched_count++;
    } else {
        waiting = 20;
    }

    update_status();
}

static void
on_key_down(const event_st *event)
{
    int key_code = event->payload.key.key_code;

    switch (key_code) {
        case KEY_LEFT: update_current_cell(-1, 0); return;
        case KEY_RIGHT: update_current_cell(1, 0); return;
        case KEY_UP: update_current_cell(0, -1); return;
        case KEY_DOWN: update_current_cell(0, 1); return;
        case KEY_SPACE:
        case KEY_ENTER: on_enter(); return;
    }
}

static void
on_key_up(const event_st *event)
{
    int ch = event->payload.key.key_char;

    if (ch == 'r' && matched_count == PAIR_COUNT) {
        restart_game();
        return;
    }
}

static void
init_grid(void)
{
    grid.cell_width = GRID_CELL_WIDTH;
    grid.cell_height = GRID_CELL_HEIGHT;
    grid.cols = GRID_COLS;
    grid.rows = GRID_ROWS;
    grid.x = GRID_X;
    grid.y = GRID_Y;
}

static void
on_show(void)
{
    static int initialized = 0;

    if (!initialized) {
        gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

        init_grid();

        app_pairs.on_tick = on_tick;
        app_pairs.on_key_down = on_key_down;
        app_pairs.on_key_up = on_key_up;

        initialized = 1;
    }

    gui_window_draw(&window, COLOR_FG, 1);
    restart_game();
}

global app_st app_pairs = {
    "Pairs",
    &icon_pairs,
    on_show,
};
