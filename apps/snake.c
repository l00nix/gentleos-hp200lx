// --------------------------------------------------------------------------------------
// Copyright (c) 2025-2026 luke8086
// Distributed under the terms of GPL-2 License
// --------------------------------------------------------------------------------------
// File: snake.c - Snake game
// --------------------------------------------------------------------------------------

#include "kernel.h"
#include <gui.h>

enum {
    GRID_CELL_WIDTH = 12,
    GRID_CELL_HEIGHT = 12,
    GRID_ROWS = 14,
    GRID_COLS = 24,
    GRID_COUNT = GRID_ROWS * GRID_COLS,
    GRID_WIDTH = GRID_WIDTH_SPACED(GRID_CELL_WIDTH, GRID_COLS),
    GRID_HEIGHT = GRID_HEIGHT_SPACED(GRID_CELL_HEIGHT, GRID_ROWS),
    GRID_X = 1,
    GRID_Y = TITLE_BAR_HEIGHT,

    WINDOW_WIDTH = GRID_X + GRID_WIDTH + 1,
    WINDOW_HEIGHT = GRID_Y + GRID_HEIGHT + 1,

    TIMEOUT_DURATION = 120,
};

static uint8_t window_pixels[WINDOW_WIDTH * WINDOW_HEIGHT];
static surface_st window_surface;
static window_st window;

static widget_st title_bar;
static widget_st *widgets[1];

static grid_st grid;

enum {
    CELL_FLOOR = 0,
    CELL_WALL = 1,
    CELL_SNAKE = 2,
    CELL_FRUIT = 3,
};

uint8_t cell_colors[4] = { COLOR_BG, COLOR_FG, COLOR_FG, COLOR_FG };

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
static uint64_t timeout_id = 0;

static void on_timeout(void *);

static int
is_game_paused(void)
{
    return timeout_id == 0;
}

static void
update_status(void)
{
    const char *msg = is_game_paused() ? "  \xb3  Press 'p' to resume" : "";

    gui_status_set("Score: %d  Best: %d%s", score, best_score, msg);
}

static void
pause_game(void)
{
    if (is_game_paused()) {
        return;
    }

    gui_timeout_remove(timeout_id);
    timeout_id = 0;

    update_status();
}

static void
resume_game(void)
{
    if (!is_game_paused()) {
        return;
    }

    timeout_id = gui_timeout_add(TIMEOUT_DURATION, on_timeout, NULL);

    update_status();
}

static void
draw_cell(int x, int y, uint8_t cell_type)
{
    cells[x][y] = cell_type;

    rect_st r = gui_grid_cell_rect(&grid, x, y);
    gui_surface_draw_rect(window.surface, r, cell_colors[cell_type]);
    gui_wm_render_window_region(&window, r);
}

static void
draw_region(int x, int y, int w, int h, uint8_t cell_type)
{
    for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; ++i) {
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
    if (body.grow) {
        ++body.tail;
        --body.grow;
        update_status();
    } else {
        draw_cell(body.tail->x, body.tail->y, CELL_FLOOR);
    }

    for (coords_st *c = body.tail; c != body.head; --c) {
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
    resume_game();
}

static void
on_timeout(void *unused _unsd)
{
    if (!window.visible) {
        return;
    }

    timeout_id = gui_timeout_add(TIMEOUT_DURATION, on_timeout, NULL);

    coords_st next_head = move_head(*body.head);

    if (next_head.x < 0 || next_head.x >= GRID_COLS ||
        next_head.y < 0 || next_head.y >= GRID_ROWS) {
        restart_game();
        return;
    }

    uint8_t next_block = cells[next_head.x][next_head.y];

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
on_keyboard(window_st *window _unsd, event_st event)
{
    if (event.key_char == 'p') {
        if (is_game_paused()) {
            resume_game();
        } else {
            pause_game();
        }
        return;
    }

    int key = event.key_code;

    if (key == KEY_UP && prev_dir != DIR_DOWN) next_dir = DIR_UP;
    else if (key == KEY_DOWN && prev_dir != DIR_UP) next_dir = DIR_DOWN;
    else if (key == KEY_LEFT && prev_dir != DIR_RIGHT) next_dir = DIR_LEFT;
    else if (key == KEY_RIGHT && prev_dir != DIR_LEFT) next_dir = DIR_RIGHT;
}

static void
on_close(window_st *window _unsd)
{
    pause_game();
}

static void
init_window(void)
{
    window_surface.size.width = WINDOW_WIDTH;
    window_surface.size.height = WINDOW_HEIGHT;
    window_surface.pitch = WINDOW_WIDTH;
    window_surface.pixels = window_pixels;

    window.size.width = WINDOW_WIDTH;
    window.size.height = WINDOW_HEIGHT;
    window.surface = &window_surface;
    window.title = "Snake";
    window.bg_color = COLOR_BG;
    window.widgets = widgets;
    window.widgets_capacity = sizeof(widgets) / sizeof(widgets[0]);
    window.on_key_down = on_keyboard;
    window.on_close = on_close;

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

    gui_window_init_frame(&window, &title_bar);
    restart_game();

    (void)gui_wm_add_window(&window);
}

app_st app_snake = {
    .icon = &bitmap_icon_snake,
    .show = show_app,
};
