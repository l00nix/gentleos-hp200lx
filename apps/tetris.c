/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: tetris.c - Tetris game
 */

#include <gui.h>

enum {
    GRID_CELL_WIDTH = 6,
    GRID_CELL_HEIGHT = 6,
    GRID_ROWS = 20,
    GRID_COLS = 10,
    GRID_WIDTH = GRID_WIDTH_SPACED(GRID_CELL_WIDTH, GRID_COLS),
    GRID_HEIGHT = GRID_HEIGHT_SPACED(GRID_CELL_HEIGHT, GRID_ROWS),
    GRID_X = 1,
    GRID_Y = TITLE_BAR_HEIGHT,

    WINDOW_WIDTH = GRID_X + GRID_WIDTH + 1,
    WINDOW_HEIGHT = GRID_Y + GRID_HEIGHT + 1,

    DROP_INTERVAL = 300,
};

static window_st window;

static grid_st grid;

static uint16_t pieces[7][4] = {
    { 0x4444, 0x0f00, 0x4444, 0x0f00 }, /* I */
    { 0x44c0, 0x8e00, 0x6440, 0x0e20 }, /* J */
    { 0x4460, 0x0e80, 0xc440, 0x2e00 }, /* L */
    { 0x0cc0, 0x0cc0, 0x0cc0, 0x0cc0 }, /* O */
    { 0x06c0, 0x4620, 0x06c0, 0x4620 }, /* S */
    { 0x4e00, 0x4640, 0x0e40, 0x4c40 }, /* T */
    { 0x0c60, 0x2640, 0x0c60, 0x2640 }, /* Z */
};

static uint8_t board[GRID_ROWS][GRID_COLS];
static int cur_piece;
static int cur_rot;
static int cur_col;
static int cur_row;
static int game_over;
static uint16_t score;
static uint16_t best_score;
static uint32_t timeout_id;

static void on_timeout(void *);

static int
is_game_paused(void)
{
    return timeout_id == 0;
}

static void
update_status(void)
{
    const char *paused_msg = is_game_paused() ? "  \xb3  Press 'p' to resume" : "";

    if (game_over) {
        gui_status_set("Game Over!  Score: %u  Best: %u", score, best_score);
    } else {
        gui_status_set("Score: %u  Best: %u%s", score, best_score, paused_msg);
    }
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

    timeout_id = gui_timeout_add(DROP_INTERVAL, on_timeout, NULL);

    update_status();
}

static void
update_score(int ds)
{
    score += ds;
    update_status();
}

static void
draw_cell(int row, int col, int active)
{
    rect_st cell;
    gui_grid_cell_rect(&grid, col, row, &cell);
    gui_surface_draw_rect(&window.origin, &cell, active ? COLOR_FG : COLOR_BG);
    gui_wm_render_window_region(&window.origin, &cell);
}

static int
is_piece_valid(int piece_idx, int row, int col, int rot)
{
    uint16_t piece = pieces[piece_idx][rot];
    int dx, dy, x, y;

    for (dy = 0; dy < 4; ++dy) {
        for (dx = 0; dx < 4; ++dx) {
            if (!(piece & (0x8000 >> (dy * 4 + dx)))) {
                continue;
            }

            x = col + dx;
            y = row + dy;

            if (x < 0 || x >= GRID_COLS || y < 0 || y >= GRID_ROWS) {
                return 0;
            }

            if (board[y][x]) {
                return 0;
            }
        }
    }

    return 1;
}

static void
draw_current_piece(int visible)
{
    uint16_t piece = pieces[cur_piece][cur_rot];
    int dx, dy, col, row;

    for (dy = 0; dy < 4; ++dy) {
        for (dx = 0; dx < 4; ++dx) {
            if (!(piece & (0x8000 >> (dy * 4 + dx)))) {
                continue;
            }

            col = cur_col + dx;
            row = cur_row + dy;

            if (col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS) {
                draw_cell(row, col, visible);
            }
        }
    }
}

static void
lock_current_piece(void)
{
    uint16_t piece = pieces[cur_piece][cur_rot];
    int dx, dy, col, row;

    for (dy = 0; dy < 4; ++dy) {
        for (dx = 0; dx < 4; ++dx) {
            if (!(piece & (0x8000 >> (dy * 4 + dx)))) {
                continue;
            }

            col = cur_col + dx;
            row = cur_row + dy;

            if (col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS) {
                board[row][col] = 1;
            }
        }
    }

    update_score(5);
}

static int
move_current_piece(int dy, int dx, int dr)
{
    int row = cur_row + dy;
    int col = cur_col + dx;
    int rot = (cur_rot + dr) % 4;

    if (!is_piece_valid(cur_piece, row, col, rot)) {
        return 0;
    }

    draw_current_piece(0);
    cur_col = col;
    cur_row = row;
    cur_rot = rot;
    draw_current_piece(1);

    return 1;
}

static int
is_row_full(int row)
{
    int col;

    for (col = 0; col < GRID_COLS; ++col) {
        if (!board[row][col]) {
            return 0;
        }
    }

    return 1;
}

static void
clear_rows(void)
{
    int row, row_to_shift, row_to_draw, col;

    for (row = GRID_ROWS - 1; row >= 0; --row) {
        if (!is_row_full(row)) {
            continue;
        }

        for (row_to_shift = row; row_to_shift > 0; --row_to_shift) {
            for (col = 0; col < GRID_COLS; ++col) {
                board[row_to_shift][col] = board[row_to_shift - 1][col];
            }
        }

        for (col = 0; col < GRID_COLS; ++col) {
            board[0][col] = 0;
        }

        for (row_to_draw = 0; row_to_draw <= row; ++row_to_draw) {
            for (col = 0; col < GRID_COLS; ++col) {
                draw_cell(row_to_draw, col, board[row_to_draw][col]);
            }
        }

        update_score(20);
        ++row;
    }
}

static void
spawn_piece(void)
{
    cur_piece = rand() % 7;
    cur_rot = 0;
    cur_col = GRID_COLS / 2 - 1;
    cur_row = 0;

    if (!is_piece_valid(cur_piece, cur_row, cur_col, cur_rot)) {
        game_over = 1;

        if (score > best_score) {
            best_score = score;
        }

        update_status();
        return;
    }

    draw_current_piece(1);
}

static void
restart_game(void)
{
    int row, col;

    game_over = 0;
    score = 0;

    for (row = 0; row < GRID_ROWS; ++row) {
        for (col = 0; col < GRID_COLS; ++col) {
            board[row][col] = 0;
            draw_cell(row, col, 0);
        }
    }

    spawn_piece();
    resume_game();
    update_status();
}

static void
on_timeout(void *unused _unsd)
{
    if (!window.visible) {
        return;
    }

    timeout_id = gui_timeout_add(DROP_INTERVAL, on_timeout, NULL);

    if (game_over) {
        return;
    }

    if (!move_current_piece(1, 0, 0)) {
        lock_current_piece();
        clear_rows();
        spawn_piece();
    }
}

static void
on_keyboard(window_st *w _unsd, const event_st *event)
{
    int key_char = event->payload.key.key_char;
    int key_code = event->payload.key.key_code;

    if (game_over) {
        restart_game();
        return;
    }

    if (key_char == 'p') {
        if (is_game_paused()) {
            resume_game();
        } else {
            pause_game();
        }
        return;
    }

    if (is_game_paused()) {
        return;
    }

    if (key_code == KEY_LEFT) {
        move_current_piece(0, -1, 0);
    } else if (key_code == KEY_RIGHT) {
        move_current_piece(0, 1, 0);
    } else if (key_code == KEY_DOWN) {
        move_current_piece(1, 0, 0);
    } else if (key_code == KEY_UP) {
        move_current_piece(0, 0, 1);
    } else if (key_char == ' ') {
        while (move_current_piece(1, 0, 0)) {
            /* drop */
        };
    }
}

static void
on_close(window_st *win _unsd)
{
    pause_game();
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.title = "Tetris";
    window.bg_color = COLOR_BG;
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

    if (timeout_id) {
        gui_timeout_remove(timeout_id);
        timeout_id = 0;
    }

    gui_wm_add_window(&window);
    gui_window_draw(&window);
    restart_game();
}

app_st app_tetris = {
    &bitmap_icon_tetris,
    show_app,
};
