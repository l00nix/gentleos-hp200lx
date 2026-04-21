/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: mahjong.c - Mahjong game
 */

#include <gui.h>

enum {
    TILE_W = 22,
    TILE_H = 20,
    TILE_D = 2,

    TILE_TYPE_COUNT = 36,
    TILES_PER_TYPE = 4,
    TILE_EMPTY = 0,
    TILE_COUNT = 144,

    BOARD_LAYERS = 4,
    BOARD_COLS = 14,
    BOARD_ROWS = 8,

    WINDOW_WIDTH = BOARD_COLS * (TILE_W - 1) + 1 + TILE_D,
    WINDOW_HEIGHT = BOARD_ROWS * (TILE_H - 1) + 1 + TILE_D,

    STATE_DEFAULT = 0,
    STATE_WON = 1,
    STATE_STUCK = 2,
};

static bitmap_st *tile_bitmaps[] = {
    NULL,
    &sprite_mj_ci_1, &sprite_mj_ci_2, &sprite_mj_ci_3, &sprite_mj_ci_4,
    &sprite_mj_ci_5, &sprite_mj_ci_6, &sprite_mj_ci_7, &sprite_mj_ci_8, &sprite_mj_ci_9,
    &glyph_mn_bbchick, &sprite_mj_ba_2, &sprite_mj_ba_3, &sprite_mj_ba_4,
    &sprite_mj_ba_5, &sprite_mj_ba_6, &sprite_mj_ba_7, &sprite_mj_ba_8, &sprite_mj_ba_9,
    &glyph_mn_num_1, &glyph_mn_num_2, &glyph_mn_num_3, &glyph_mn_num_4,
    &glyph_mn_num_5, &glyph_mn_num_6, &glyph_mn_num_7, &glyph_mn_num_8, &glyph_mn_num_9,
    &glyph_mn_east, &glyph_mn_south, &glyph_mn_west, &glyph_mn_north,
    &glyph_mn_white, &glyph_mn_issue, &glyph_mn_central,
    &glyph_mn_tulip, &glyph_mn_herb,
};

/* Layer 0: 88, Layer 1: 44, Layer 2: 8, Layer 3: 4 */
static const uint8_t board_layout[BOARD_LAYERS][BOARD_ROWS][BOARD_COLS] = {
    {
        {0,0,0,1,1,1,1,1,1,1,1,0,0,0},
        {0,0,1,1,1,1,1,1,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,1,1,1,1,1,1,0},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {0,1,1,1,1,1,1,1,1,1,1,1,1,0},
        {0,0,1,1,1,1,1,1,1,1,1,1,0,0},
        {0,0,0,1,1,1,1,1,1,1,1,0,0,0},
    },
    {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,1,1,1,1,1,1,0,0,0,0},
        {0,0,0,1,1,1,1,1,1,1,1,0,0,0},
        {0,0,0,1,1,1,1,1,1,1,1,0,0,0},
        {0,0,0,1,1,1,1,1,1,1,1,0,0,0},
        {0,0,0,1,1,1,1,1,1,1,1,0,0,0},
        {0,0,0,0,1,1,1,1,1,1,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    },
    {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,1,1,1,1,0,0,0,0,0},
        {0,0,0,0,0,1,1,1,1,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    },
    {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,1,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,1,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    },
};

static uint8_t board[BOARD_LAYERS][BOARD_ROWS][BOARD_COLS];

static int cur_col;
static int cur_row;

static int sel_col;
static int sel_row;
static int sel_layer;

static int remaining_pairs;
static int state;

static window_st window;

static void
update_status(void)
{
    if (state == STATE_WON) {
        gui_status_set("You Won! Press r to play again");
    } else if (state == STATE_STUCK) {
        gui_status_set("No moves! [S]huffle or [R]estart");
    } else {
        gui_status_set("Pairs: %d  [S]huffle [R]estart", remaining_pairs);
    }
}

static int
topmost_layer_at(int col, int row)
{
    int layer;

    for (layer = BOARD_LAYERS - 1; layer >= 0; layer--) {
        if (board[layer][row][col] != TILE_EMPTY) {
            return layer;
        }
    }

    return -1;
}

static int
is_tile_free(int layer, int col, int row)
{
    int left_empty, right_empty;

    if (board[layer][row][col] == TILE_EMPTY) {
        return 0;
    }

    if (layer != topmost_layer_at(col, row)) {
        return 0;
    }

    left_empty = (col == 0 || board[layer][row][col - 1] == TILE_EMPTY);
    right_empty = (col == BOARD_COLS - 1 || board[layer][row][col + 1] == TILE_EMPTY);

    return left_empty || right_empty;
}

static int
has_valid_moves(void)
{
    uint8_t free_counts[TILE_TYPE_COUNT + 1];
    int layer, col, row;
    uint8_t type;

    memset(free_counts, 0, sizeof(free_counts));

    for (layer = 0; layer < BOARD_LAYERS; ++layer) {
        for (row = 0; row < BOARD_ROWS; ++row) {
            for (col = 0; col < BOARD_COLS; ++col) {
                if (is_tile_free(layer, col, row)) {
                    type = board[layer][row][col];
                    if (++free_counts[type] >= 2) {
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}

static void
draw_tile(int layer, int col, int row)
{
    uint8_t type = board[layer][row][col];
    int x = col * (TILE_W - 1) - layer * TILE_D;
    int y = row * (TILE_H - 1) - layer * TILE_D;
    int is_cursor = (col == cur_col && row == cur_row &&
        layer == topmost_layer_at(col, row));
    int is_selected = (col == sel_col && row == sel_row && layer == sel_layer);
    uint8_t face_color = is_selected ? COLOR_FG : COLOR_BG;
    uint8_t glyph_color = is_selected ? COLOR_BG : COLOR_FG;
    rect_st rect, inner;
    int i;

    for (i = 1; i <= TILE_D; ++i) {
        gui_rect_init(&rect, x + i, y + i, TILE_W, TILE_H);
        gui_surface_draw_rect(&window.origin, &rect, COLOR_FG);
    }

    gui_rect_init(&rect, x, y, TILE_W, TILE_H);
    gui_surface_draw_rect(&window.origin, &rect, face_color);
    gui_surface_draw_border(&window.origin, &rect, COLOR_FG);
    gui_rect_init(&inner, x + 1, y + 1, TILE_W - 2, TILE_H - 2);
    gui_surface_draw_bitmap_centered(&window.origin, &window.size,
        &inner, tile_bitmaps[type], glyph_color);

    if (is_cursor) {
        gui_rect_init(&inner, x + 2, y + 2, TILE_W - 4, TILE_H - 4);
        gui_surface_draw_border(&window.origin, &inner, glyph_color);
    }
}

static void
draw_board(void)
{
    int layer, row, col;
    rect_st rect;

    gui_rect_init(&rect, 0, 0, window.size.width, window.size.height);
    gui_surface_draw_rect(&window.origin, &rect, COLOR_BG);

    for (layer = 0; layer < BOARD_LAYERS; ++layer) {
        for (row = 0; row < BOARD_ROWS; ++row) {
            for (col = 0; col < BOARD_COLS; ++col) {
                if (board[layer][row][col] != TILE_EMPTY) {
                    draw_tile(layer, col, row);
                }
            }
        }
    }

    /* Cursor on empty position */
    if (topmost_layer_at(cur_col, cur_row) < 0) {
        gui_rect_init(&rect, cur_col * (TILE_W - 1),
            cur_row * (TILE_H - 1), TILE_W, TILE_H);
        gui_surface_draw_border(&window.origin, &rect, COLOR_FG);
    }

    gui_rect_init(&rect, 0, 0, window.size.width, window.size.height);
    gui_surface_mark_dirty(&window.origin, &rect);
}

static void
shuffle_tiles(void)
{
    uint8_t deck[TILE_COUNT];
    uint8_t tmp;
    int count = 0;
    int layer, col, row, i, j;

    if (state == STATE_WON) {
        return;
    }

    for (layer = 0; layer < BOARD_LAYERS; ++layer) {
        for (row = 0; row < BOARD_ROWS; ++row) {
            for (col = 0; col < BOARD_COLS; ++col) {
                if (board[layer][row][col] != TILE_EMPTY) {
                    deck[count++] = board[layer][row][col];
                }
            }
        }
    }

    for (i = count - 1; i > 0; --i) {
        j = rand() % (i + 1);
        tmp = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp;
    }

    i = 0;
    for (layer = 0; layer < BOARD_LAYERS; ++layer) {
        for (row = 0; row < BOARD_ROWS; ++row) {
            for (col = 0; col < BOARD_COLS; ++col) {
                if (board[layer][row][col] != TILE_EMPTY) {
                    board[layer][row][col] = deck[i++];
                }
            }
        }
    }

    sel_col = -1;
    state = has_valid_moves() ? STATE_DEFAULT : STATE_STUCK;

    draw_board();
    update_status();
}

static void
init_tiles(void)
{
    int layer, col, row, i = 0;

    memset(board, 0, sizeof(board));

    for (layer = 0; layer < BOARD_LAYERS; layer++) {
        for (row = 0; row < BOARD_ROWS; row++) {
            for (col = 0; col < BOARD_COLS; col++) {
                if (board_layout[layer][row][col]) {
                    board[layer][row][col] = i / TILES_PER_TYPE + 1;
                    ++i;
                }
            }
        }
    }

    shuffle_tiles();
}

static void
select_tile(void)
{
    int layer;

    if (state != STATE_DEFAULT) {
        return;
    }

    layer = topmost_layer_at(cur_col, cur_row);

    if (layer < 0 || !is_tile_free(layer, cur_col, cur_row)) {
        return;
    }

    /* Selecting already selected tile */
    if (sel_col == cur_col && sel_row == cur_row && sel_layer == layer) {
        sel_col = -1;
        draw_board();
        update_status();
        return;
    }

    /* First pick */
    if (sel_col == -1) {
        sel_col = cur_col;
        sel_row = cur_row;
        sel_layer = layer;
        draw_board();
        update_status();
        return;
    }

    /* Second pick - no match */
    if (board[layer][cur_row][cur_col] != board[sel_layer][sel_row][sel_col]) {
        sel_col = -1;
        draw_board();
        gui_status_set("No match");
        return;
    }

    /* Second pick - match */
    board[layer][cur_row][cur_col] = TILE_EMPTY;
    board[sel_layer][sel_row][sel_col] = TILE_EMPTY;
    sel_col = -1;
    --remaining_pairs;

    if (remaining_pairs == 0) {
        state = STATE_WON;
    } else if (!has_valid_moves()) {
        state = STATE_STUCK;
    }

    draw_board();
    update_status();
}

static void
update_cursor(int dx, int dy)
{
    cur_col = MAX(0, MIN(BOARD_COLS - 1, cur_col + dx));
    cur_row = MAX(0, MIN(BOARD_ROWS - 1, cur_row + dy));

    draw_board();
}

static void
restart_game(void)
{
    cur_col = BOARD_COLS / 2;
    cur_row = BOARD_ROWS / 2;
    sel_col = -1;
    remaining_pairs = TILE_COUNT / 2;
    state = STATE_DEFAULT;

    init_tiles();
}

static void
on_key_down(const event_st *event)
{
    int key_code = event->payload.key.key_code;
    int key_ch = event->payload.key.key_char;

    if (key_ch == 'r') {
        restart_game();
        return;
    }

    if (key_ch == 's' && state != STATE_WON) {
        shuffle_tiles();
        return;
    }

    if (state != STATE_DEFAULT) {
        return;
    }

    switch (key_code) {
        case KEY_LEFT: update_cursor(-1, 0); return;
        case KEY_RIGHT: update_cursor(1, 0); return;
        case KEY_UP: update_cursor(0, -1); return;
        case KEY_DOWN: update_cursor(0, 1); return;
        case KEY_SPACE:
        case KEY_ENTER: select_tile(); return;
    }
}

static void
on_show(void)
{
    static int initialized = 0;

    if (!initialized) {
        gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

        app_mahjong.on_key_down = on_key_down;

        initialized = 1;
    }

    restart_game();
}

global app_st app_mahjong = {
    "Mahjong",
    &glyph_mn_east_icn,
    &window,
    on_show,
};
