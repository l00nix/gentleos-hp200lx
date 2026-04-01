/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: mines.c - Minesweeper game
 */

#include <gui.h>

static void reveal_cell(int col, int row);

enum {
    GRID_CELL_WIDTH = 13,
    GRID_CELL_HEIGHT = 13,
    GRID_ROWS = 10,
    GRID_COLS = 10,
    GRID_CELL_COUNT = GRID_ROWS * GRID_COLS,
    GRID_WIDTH = GRID_WIDTH_SPACED(GRID_CELL_WIDTH, GRID_COLS),
    GRID_HEIGHT = GRID_HEIGHT_SPACED(GRID_CELL_HEIGHT, GRID_ROWS),
    GRID_X = 1,
    GRID_Y = 1,

    WINDOW_WIDTH = GRID_X + GRID_WIDTH + 1,
    WINDOW_HEIGHT = GRID_Y + GRID_HEIGHT + 1,

    MINE_COUNT = 18,
};

static window_st window;

static widget_st cell_widgets[GRID_CELL_COUNT];
static widget_st *widgets[GRID_CELL_COUNT];

static grid_st grid;

enum {
    CELL_STATE_HIDDEN = 0,
    CELL_STATE_REVEALED = 1,
    CELL_STATE_FLAGGED = 2,
};

enum {
    CELL_TYPE_EMPTY = 0,
    CELL_TYPE_MINE = 9,
};

enum {
    GAME_STATE_PLAYING = 0,
    GAME_STATE_WON = 1,
    GAME_STATE_LOST = 2,
};

static uint8_t cell_state[GRID_COLS][GRID_ROWS];
static uint8_t cell_type[GRID_COLS][GRID_ROWS];

static size_t
count_cells_by_state(uint8_t state)
{
    int x, y;
    size_t count = 0;

    for (y = 0; y < GRID_ROWS; ++y) {
        for (x = 0; x < GRID_COLS; ++x) {
            if (cell_state[x][y] == state) {
                count++;
            }
        }
    }

    return count;
}

static size_t
count_adjacent_mines(int col, int row)
{
    int dx, dy, nx, ny;
    size_t count = 0;

    for (dy = -1; dy <= 1; ++dy) {
        for (dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) {
                continue;
            }

            nx = col + dx;
            ny = row + dy;

            if (nx >= 0 && nx < GRID_COLS && ny >= 0 && ny < GRID_ROWS) {
                if (cell_type[nx][ny] == CELL_TYPE_MINE) {
                    ++count;
                }
            }
        }
    }

    return count;
}

static void
draw_cell(widget_st *widget)
{
    int idx = widget->tag1;
    int col = idx % GRID_COLS;
    int row = idx / GRID_COLS;
    uint8_t state = cell_state[col][row];
    uint8_t type = cell_type[col][row];
    rect_st rect, num_rect, dot_rect;
    char num_str[2];

    num_str[0] = 0;
    num_str[1] = 0;

    gui_rect_copy(&rect, &widget->rect);
    gui_surface_draw_rect(&window.origin, &rect, COLOR_BG);

    if (state == CELL_STATE_FLAGGED) {
        gui_surface_draw_bitmap_centered(&window.origin, &window.size, &rect, &bitmap_sprite_flag,
            COLOR_FG);
    } else if (state == CELL_STATE_REVEALED && type == CELL_TYPE_MINE) {
        gui_surface_draw_bitmap_centered(&window.origin, &window.size, &rect, &bitmap_sprite_mine,
            COLOR_FG);
    } else if (state == CELL_STATE_REVEALED && type == CELL_TYPE_EMPTY) {
        gui_rect_init(&dot_rect, rect.x + rect.width / 2 - 1, rect.y + rect.height / 2, 2, 1);
        gui_surface_draw_rect(&window.origin, &dot_rect, COLOR_FG);
    } else if (state == CELL_STATE_REVEALED) {
        num_str[0] = '0' + type;

        gui_rect_init(&num_rect, rect.x + 1, rect.y + 1,
            rect.width - 1, rect.height - 1);

        gui_surface_draw_str_centered(&window.origin, &num_rect, NULL,
            num_str, COLOR_FG, COLOR_BG);
    }

    if (widget == widget->window->focused_widget) {
        gui_surface_draw_border(&window.origin, &widget->rect, COLOR_FG);
    }

    gui_wm_render_window_region(&window.origin, &rect);
}

static void
update_cell(int col, int row, uint8_t type, uint8_t state)
{
    int idx = row * GRID_COLS + col;
    cell_type[col][row] = type;
    cell_state[col][row] = state;
    draw_cell(&cell_widgets[idx]);
}

static void
update_all_mines(uint8_t state)
{
    int row, col;

    for (row = 0; row < GRID_ROWS; ++row) {
        for (col = 0; col < GRID_COLS; ++col) {
            if (cell_type[col][row] == CELL_TYPE_MINE) {
                update_cell(col, row, CELL_TYPE_MINE, state);
            }
        }
    }
}

static void
clear_cells(void)
{
    int row, col;

    for (row = 0; row < GRID_ROWS; ++row) {
        for (col = 0; col < GRID_COLS; ++col) {
            update_cell(col, row, CELL_TYPE_EMPTY, CELL_STATE_HIDDEN);
        }
    }
}

static void
place_mines(int except_col, int except_row)
{
    int remaining = MINE_COUNT;
    int col, row;

    while (remaining > 0) {
        col = rand() % GRID_COLS;
        row = rand() % GRID_ROWS;

        if (col == except_col && row == except_row) {
            continue;
        }

        if (cell_type[col][row] != CELL_TYPE_MINE) {
            cell_type[col][row] = CELL_TYPE_MINE;
            --remaining;
        }
    }
}

static int
get_game_state(void)
{
    int row, col;

    for (row = 0; row < GRID_ROWS; ++row) {
        for (col = 0; col < GRID_COLS; ++col) {
            if (cell_type[col][row] == CELL_TYPE_MINE &&
                cell_state[col][row] == CELL_STATE_REVEALED) {
                return GAME_STATE_LOST;
            }
        }
    }

    if (count_cells_by_state(CELL_STATE_REVEALED) == (GRID_CELL_COUNT - MINE_COUNT)) {
        return GAME_STATE_WON;
    }

    return GAME_STATE_PLAYING;
}

static void
update_status(void)
{
    int state = get_game_state();

    if (state == GAME_STATE_LOST) {
        gui_status_set("Game Over! Press r to restart");
    } else if (state == GAME_STATE_WON) {
        gui_status_set("You Win! Press r to restart");
    } else {
        size_t flagged_count = count_cells_by_state(CELL_STATE_FLAGGED);
        size_t remaining = MINE_COUNT > flagged_count ? MINE_COUNT - flagged_count : 0;

        gui_status_set("Remaining mines: %u", remaining);
    }
}

static void
restart_game(void)
{
    clear_cells();
    update_status();
}

static void
reveal_adjacent_cells(int col, int row)
{
    int dx, dy;

    for (dy = -1; dy <= 1; dy++) {
        for (dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) {
                continue;
            }

            reveal_cell(col + dx, row + dy);
        }
    }
}

static void
reveal_cell(int col, int row)
{
    int adjacent_mine_count;

    if (get_game_state() != GAME_STATE_PLAYING) {
        return;
    }

    if (col < 0 || col >= GRID_COLS || row < 0 || row >= GRID_ROWS) {
        return;
    }

    if (cell_state[col][row] != CELL_STATE_HIDDEN) {
        return;
    }

    if (count_cells_by_state(CELL_STATE_REVEALED) == 0) {
        place_mines(col, row);
    }

    if (cell_type[col][row] == CELL_TYPE_MINE) {
        update_all_mines(CELL_STATE_REVEALED);
        update_status();
        return;
    };

    adjacent_mine_count = count_adjacent_mines(col, row);
    update_cell(col, row, adjacent_mine_count, CELL_STATE_REVEALED);

    if (adjacent_mine_count == 0) {
        reveal_adjacent_cells(col, row);
    }

    if (get_game_state() == GAME_STATE_WON) {
        update_all_mines(CELL_STATE_FLAGGED);
    }

    update_status();
}

static void
flag_cell(int col, int row)
{
    if (get_game_state() != GAME_STATE_PLAYING) {
        return;
    }

    if (cell_state[col][row] == CELL_STATE_HIDDEN) {
        update_cell(col, row, cell_type[col][row], CELL_STATE_FLAGGED);
        update_status();
    } else if (cell_state[col][row] == CELL_STATE_FLAGGED) {
        update_cell(col, row, cell_type[col][row], CELL_STATE_HIDDEN);
        update_status();
    }
}

static void
on_cell_pointer_up(widget_st *widget, const event_st *event _unsd, const point_st *pos _unsd)
{
    reveal_cell(widget->tag1 % GRID_COLS, widget->tag1 / GRID_COLS);
}

static void
on_key_up(window_st *win _unsd, const event_st *event)
{
    int ch = event->payload.key.key_char;
    widget_st *cell = win->focused_widget;

    if (ch == 'r' && get_game_state() != GAME_STATE_PLAYING) {
        restart_game();
        return;
    }

    if (ch == 'f' && cell) {
        flag_cell(cell->tag1 % GRID_COLS, cell->tag1 / GRID_COLS);
        return;
    }
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.title = "Mines";
    window.bg_color = COLOR_FG;
    window.widgets = widgets;
    window.widgets_capacity = sizeof(widgets) / sizeof(widgets[0]);
    window.focused_widget = &cell_widgets[0];
    window.on_key_down = on_key_up;
}

static void
init_grid(void)
{
    int i, col, row;

    memset(cell_widgets, 0, sizeof(cell_widgets));

    grid.cell_width = GRID_CELL_WIDTH;
    grid.cell_height = GRID_CELL_HEIGHT;
    grid.cols = GRID_COLS;
    grid.rows = GRID_ROWS;
    grid.x = GRID_X;
    grid.y = GRID_Y;

    for (i = 0; i < GRID_CELL_COUNT; i++) {
        col = i % GRID_COLS;
        row = i / GRID_COLS;

        cell_widgets[i].type = WIDGET_TYPE_BUTTON;
        gui_grid_cell_rect(&grid, col, row, &cell_widgets[i].rect);
        cell_widgets[i].draw = draw_cell;
        cell_widgets[i].tag1 = i;
        cell_widgets[i].on_pointer_up = on_cell_pointer_up;
        cell_widgets[i].focusable = 1;
        cell_widgets[i].focus_x = col;
        cell_widgets[i].focus_y = row;

        gui_window_add_widget(&window, &cell_widgets[i]);
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

app_st app_mines = {
    &bitmap_icon_mines,
    show_app,
};
