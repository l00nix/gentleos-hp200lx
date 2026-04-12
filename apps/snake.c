/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: snake.c - Snake game
 */

#include "kernel.h"
#include <gui.h>

enum {
    GRID_CELL_WIDTH = 6,
    GRID_CELL_HEIGHT = 6,
    GRID_ROWS = 16,
    GRID_COLS = 28,
    GRID_COUNT = GRID_ROWS * GRID_COLS,
    GRID_WIDTH = GRID_WIDTH_SPACED(GRID_CELL_WIDTH, GRID_COLS),
    GRID_HEIGHT = GRID_HEIGHT_SPACED(GRID_CELL_HEIGHT, GRID_ROWS),
    GRID_X = 1,
    GRID_Y = 1,

    WINDOW_WIDTH = GRID_X + GRID_WIDTH + 1,
    WINDOW_HEIGHT = GRID_Y + GRID_HEIGHT + 1,
};

static window_st window;

static grid_st grid;

enum {
    CELL_FLOOR = 0,
    CELL_WALL = 1,
    CELL_SNAKE = 2,
    CELL_FRUIT = 3,
};

static uint8_t cell_colors[4] = { COLOR_BG, COLOR_FG, COLOR_FG, COLOR_FG };

static uint8_t cells[GRID_COLS][GRID_ROWS];

typedef struct {
    int x, y;
} coords_st;

static struct {
    coords_st coords[GRID_COLS * GRID_ROWS];
    coords_st *head;
    coords_st *tail;
    int grow;
} body;

static enum {
    DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT
} prev_dir, next_dir;

static int score = 0;
static int best_score = 0;

static void
update_status(void)
{
    gui_status_set("Score: %d  Best: %d", score, best_score);
}

static void
draw_cell(int x, int y, uint8_t cell_type)
{
    rect_st r;

    cells[x][y] = cell_type;

    gui_grid_cell_rect(&grid, x, y, &r);
    gui_surface_draw_rect(&window.origin, &r, cell_colors[cell_type]);
    gui_wm_render_window_region(&window.origin, &r);
}

static void
draw_region(int x, int y, int w, int h, uint8_t cell_type)
{
    int i, j;

    for (j = 0; j < h; ++j) {
        for (i = 0; i < w; ++i) {
            draw_cell(x + i, y + j, cell_type);
        }
    }
}

static void
draw_board(void)
{
    draw_region(0, 0, GRID_COLS, GRID_ROWS, CELL_FLOOR);
}

static void
add_fruit(void) {
    coords_st c;

    do {
        c.x = rand() % GRID_COLS;
        c.y = rand() % GRID_ROWS;
    } while (cells[c.x][c.y] != CELL_FLOOR);

    draw_cell(c.x, c.y, CELL_FRUIT);
}

static coords_st
move_head(coords_st head)
{
    switch (next_dir) {
    case DIR_UP:    head.y--; break;
    case DIR_DOWN:  head.y++; break;
    case DIR_LEFT:  head.x--; break;
    case DIR_RIGHT: head.x++; break;
    }

    return head;
}

static void
move_snake(coords_st next_head)
{
    coords_st *c;

    if (body.grow) {
        ++body.tail;
        --body.grow;
        update_status();
    } else {
        draw_cell(body.tail->x, body.tail->y, CELL_FLOOR);
    }

    for (c = body.tail; c != body.head; --c) {
        *c = *(c - 1);
    }

    *(body.head) = next_head;

    draw_cell(next_head.x, next_head.y, CELL_SNAKE);
}

static void
restart_game(void)
{
    if (score > best_score) {
        best_score = score;
    }

    score = 0;

    body.coords[0].x = GRID_COLS / 2;
    body.coords[0].y = GRID_ROWS / 2;
    body.head = body.tail = body.coords;
    body.grow = 7;

    prev_dir = DIR_RIGHT;
    next_dir = DIR_RIGHT;

    draw_board();
    add_fruit();
    update_status();
}

static void
on_timeout(void)
{
    coords_st next_head;
    uint8_t next_block;

    if (!window.visible) {
        return;
    }

    next_head = move_head(*body.head);

    if (next_head.x < 0 || next_head.x >= GRID_COLS ||
        next_head.y < 0 || next_head.y >= GRID_ROWS) {
        restart_game();
        return;
    }

    next_block = cells[next_head.x][next_head.y];

    if (next_block != CELL_FRUIT && next_block != CELL_FLOOR) {
        restart_game();
        return;
    }

    if (next_block == CELL_FRUIT) {
        body.grow += 2;
        score += 5;
        update_status();
    }

    move_snake(next_head);

    if (next_block == CELL_FRUIT) {
        add_fruit();
    }

    prev_dir = next_dir;
}

static void
on_tick(window_st *window)
{
    static unsigned count = 0;

    ++count;

    if (count >= 3) {
        on_timeout();
        count = 0;
    }
}

static void
on_keyboard(window_st *window, const event_st *event)
{
    int key = event->payload.key.key_code;

    if (key == KEY_UP && prev_dir != DIR_DOWN) next_dir = DIR_UP;
    else if (key == KEY_DOWN && prev_dir != DIR_UP) next_dir = DIR_DOWN;
    else if (key == KEY_LEFT && prev_dir != DIR_RIGHT) next_dir = DIR_LEFT;
    else if (key == KEY_RIGHT && prev_dir != DIR_LEFT) next_dir = DIR_RIGHT;
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.bg_color = COLOR_BG;
    window.on_tick = on_tick;
    window.on_key_down = on_keyboard;
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

global app_st app_snake = {
    "Snake",
    &bitmap_i_snake,
    show_app,
};
