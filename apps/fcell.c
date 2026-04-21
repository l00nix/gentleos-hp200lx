/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: fcell.c - FreeCell game
 */

#include <gui.h>

enum {
    CARD_WIDTH = 30,
    CARD_HEIGHT = 24,

    CARD_COUNT = 52,
    HOLD_COUNT = 4,
    FOUND_COUNT = 4,
    COLUMN_COUNT = 8,

    GAP_Y = 6,
    GAP_X = 6,

    COLUMN_CARDS_STEP = 10,
    COLUMN_CARDS_MAX = 21,
    CARD_EMPTY = 0xFF,

    WINDOW_WIDTH = COLUMN_COUNT * CARD_WIDTH + (COLUMN_COUNT - 1) * GAP_X,
    WINDOW_HEIGHT = GUI_HEIGHT - 2 * STATUS_HEIGHT,

    HOLDS_Y = GAP_Y,
    COLUMNS_Y = HOLDS_Y + CARD_HEIGHT + GAP_Y,
    COLUMNS_H = WINDOW_HEIGHT - COLUMNS_Y - GAP_Y,

    PILE_NONE = 0,
    PILE_HOLDS = 1,
    PILE_FOUNDS = 2,
    PILE_COLUMNS = 3,

    STATE_DEFAULT = 0,
    STATE_ENTER_MOVE_COUNT = 1,
    STATE_WON = 2,
    STATE_HELP = 3,

    HELP_LINE_COUNT = 5,
    HELP_LINE_HEIGHT = 11,
    HELP_PADDING = 10,
    HELP_WIDTH = 180,
    HELP_HEIGHT = HELP_LINE_COUNT * HELP_LINE_HEIGHT + 2 * HELP_PADDING,
    HELP_X = (WINDOW_WIDTH - HELP_WIDTH) / 2,
    HELP_Y = (WINDOW_HEIGHT - HELP_HEIGHT) / 2,
};

static const char *rank_str[] = {
    "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"
};

static bitmap_st *suit_bmp[] = {
    &sprite_heart_2,
    &sprite_diamnd_2,
    &sprite_club_2,
    &sprite_spade_2,
};

static const char *help_lines[] = {
    "Arrows: move cursor",
    "Space:  select / place",
    "F:      move to foundation",
    "R:      restart game",
    "H:      toggle help",
};

static window_st window;

static uint8_t holds[HOLD_COUNT];
static uint8_t founds[FOUND_COUNT];
static uint8_t columns[COLUMN_COUNT][COLUMN_CARDS_MAX];
static int column_counts[COLUMN_COUNT];

static int sel_pile;
static int sel_idx;

static int cur_pile;
static int cur_idx;

static int state;

static int
card_rank(uint8_t card)
{
    return card % 13;
}

static int
card_suit(uint8_t card)
{
    return card / 13;
}

static int
card_color(uint8_t card)
{
    return card_suit(card) / 2;
}

static int
col_x(int col)
{
    return col * (CARD_WIDTH + GAP_X);
}

static void
deal_cards(void)
{
    uint8_t deck[CARD_COUNT];
    int i, j, col;
    uint8_t tmp;

    for (i = 0; i < CARD_COUNT; i++) {
        deck[i] = i;
    }

    for (i = CARD_COUNT - 1; i > 0; i--) {
        j = rand() % (i + 1);
        tmp = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp;
    }

    for (i = 0; i < COLUMN_COUNT; i++) {
        column_counts[i] = 0;
    }

    for (i = 0; i < HOLD_COUNT; i++) {
        holds[i] = CARD_EMPTY;
    }

    for (i = 0; i < FOUND_COUNT; i++) {
        founds[i] = CARD_EMPTY;
    }

    for (i = 0; i < CARD_COUNT; i++) {
        col = i % COLUMN_COUNT;
        columns[col][column_counts[col]++] = deck[i];
    }

    sel_pile = PILE_NONE;
    sel_idx = 0;

    cur_pile = PILE_COLUMNS;
    cur_idx = 0;

    state = STATE_DEFAULT;
}

static void
draw_card(int x, int y, uint8_t card, int is_sel)
{
    rect_st r;
    int rank, suit;
    uint8_t fg, bg;

    fg = is_sel ? COLOR_BG : COLOR_FG;
    bg = is_sel ? COLOR_FG : COLOR_BG;
    rank = card_rank(card);
    suit = card_suit(card);

    gui_rect_init(&r, x, y, CARD_WIDTH, CARD_HEIGHT);
    gui_surface_draw_rect(&window.origin, &r, bg);
    gui_surface_draw_border(&window.origin, &r, COLOR_FG);

    if (card == CARD_EMPTY) {
        return;
    }

    gui_surface_draw_str(&window.origin, x + 3, y + 3, &fonts[2], rank_str[rank], fg, bg);
    gui_surface_draw_bitmap(&window.origin, &window.size,
        x + CARD_WIDTH - 8, y + 3, suit_bmp[suit], fg);
}

static void
draw_cell(int pile, int idx)
{
    int x, y;
    rect_st r;
    uint8_t card;
    int is_sel;

    if (pile == PILE_HOLDS) {
        x = col_x(idx) - GAP_X;
        card = holds[idx];
    } else {
        x = col_x(idx + HOLD_COUNT) + GAP_X;
        card = founds[idx];
    }

    y = HOLDS_Y;

    gui_rect_init(&r, x, y, CARD_WIDTH, CARD_HEIGHT);
    gui_surface_draw_rect(&window.origin, &r, COLOR_BG);

    is_sel = (sel_pile == pile && sel_idx == idx);

    if (card == CARD_EMPTY) {
        gui_surface_draw_border(&window.origin, &r, COLOR_FG);
    } else {
        draw_card(x, y, card, is_sel);
    }

    gui_surface_mark_dirty(&window.origin, &r);
}

static void
draw_column(int col)
{
    int x, count, i, step, max_step, is_sel;
    rect_st r;

    x = col_x(col);
    count = column_counts[col];

    gui_rect_init(&r, x, COLUMNS_Y, CARD_WIDTH, COLUMNS_H);
    gui_surface_draw_rect(&window.origin, &r, COLOR_BG);

    if (count == 0) {
        draw_card(x, COLUMNS_Y, CARD_EMPTY, 0);
    } else {
        step = COLUMN_CARDS_STEP;
        if (count > 1) {
            max_step = (COLUMNS_H - CARD_HEIGHT) / (count - 1);
            step = MIN(step, max_step);
            step = MAX(step, 1);
        }

        for (i = 0; i < count; i++) {
            is_sel = (sel_pile == PILE_COLUMNS && sel_idx == col && i == count - 1);
            draw_card(x, COLUMNS_Y + i * step, columns[col][i], is_sel);
        }
    }

    gui_surface_mark_dirty(&window.origin, &r);
}

static void
draw_piles(void)
{
    int i;

    for (i = 0; i < COLUMN_COUNT; i++) {
        draw_column(i);
    }

    for (i = 0; i < HOLD_COUNT; i++) {
        draw_cell(PILE_HOLDS, i);
    }

    for (i = 0; i < FOUND_COUNT; i++) {
        draw_cell(PILE_FOUNDS, i);
    }
}

static void
draw_cursor(int visible)
{
    int x, y;
    rect_st r;
    bitmap_st *b = &sprite_arr_up;

    x = (CARD_WIDTH - b->size.width) / 2;

    if (cur_pile == PILE_HOLDS) {
        x += col_x(cur_idx) - GAP_X;
        y = HOLDS_Y + CARD_HEIGHT + 2;
    } else {
        x += col_x(cur_idx);
        y = WINDOW_HEIGHT - b->size.height - 2;
    }

    gui_rect_init(&r, x, y, b->size.width, b->size.height);

    if (visible) {
        gui_surface_draw_bitmap(&window.origin, &window.size, x, y, b, COLOR_FG);
    } else {
        gui_surface_draw_rect(&window.origin, &r, COLOR_BG);
    }

    gui_surface_mark_dirty(&window.origin, &r);
}

static void
move_cursor(int dx, int dy)
{
    int *idx, max_idx;

    draw_cursor(0);

    if (dy < 0) {
        cur_pile = PILE_HOLDS;
    } else if (dy > 0) {
        cur_pile = PILE_COLUMNS;
    }

    max_idx = (cur_pile == PILE_COLUMNS ? COLUMN_COUNT :  HOLD_COUNT) - 1;

    cur_idx += dx;
    cur_idx = MAX(0, cur_idx);
    cur_idx = MIN(cur_idx, max_idx);

    draw_cursor(1);
}

static void
update_status(void)
{
    if (state == STATE_WON) {
        gui_status_set("You Won! Press R to restart");
    } else {
        gui_status_set("[H]elp");
    }
}

static void
check_win(void)
{
    int i;

    for (i = 0; i < FOUND_COUNT; ++i) {
        if (founds[i] == CARD_EMPTY || card_rank(founds[i]) != 12) {
            return;
        }
    }

    state = STATE_WON;
}

static void
select_card(void)
{
    if (cur_pile == PILE_HOLDS && holds[cur_idx] != CARD_EMPTY) {
        sel_pile = PILE_HOLDS;
        sel_idx = cur_idx;
        draw_cell(PILE_HOLDS, cur_idx);
        update_status();
    } else if (cur_pile == PILE_COLUMNS && column_counts[cur_idx] > 0) {
        sel_pile = PILE_COLUMNS;
        sel_idx = cur_idx;
        draw_column(cur_idx);
        update_status();
    }
}

static void
deselect_card(void)
{
    int old_pile, old_idx;

    old_pile = sel_pile;
    old_idx = sel_idx;
    sel_pile = PILE_NONE;

    if (old_pile == PILE_HOLDS) {
        draw_cell(PILE_HOLDS, old_idx);
    } else if (old_pile == PILE_COLUMNS) {
        draw_column(old_idx);
    }

    update_status();
}

static void
show_error(const char *msg)
{
    deselect_card();
    gui_status_set("%s", msg);
}

static uint8_t
selected_card(void)
{
    if (sel_pile == PILE_HOLDS) {
        return holds[sel_idx];
    } else if (sel_pile == PILE_COLUMNS) {
        return columns[sel_idx][column_counts[sel_idx] - 1];
    } else {
        return CARD_EMPTY;
    }
}

static uint8_t
lift_card(void)
{
    uint8_t card = CARD_EMPTY;

    if (sel_pile == PILE_HOLDS) {
        card = holds[sel_idx];
        holds[sel_idx] = CARD_EMPTY;
        sel_pile = PILE_NONE;
        draw_cell(PILE_HOLDS, sel_idx);
    } else if (sel_pile == PILE_COLUMNS) {
        card = columns[sel_idx][column_counts[sel_idx] - 1];
        column_counts[sel_idx]--;
        sel_pile = PILE_NONE;
        draw_column(sel_idx);
    }

    return card;
}

static int
get_max_valid_sequence_len(int col)
{
    int count, i;
    uint8_t curr, prev;

    count = column_counts[col];

    if (count == 0) {
        return 0;
    }

    for (i = count - 1; i > 0; --i) {
        curr = columns[col][i];
        prev = columns[col][i - 1];

        if (card_rank(prev) != card_rank(curr) + 1) {
            break;
        }

        if (card_color(prev) == card_color(curr)) {
            break;
        }
    }

    return count - i;
}

static int
get_max_movable_cards_count(int dst_col)
{
    int i;
    int avail_holds = 0;
    int avail_cols = 0;

    for (i = 0; i < HOLD_COUNT; ++i) {
        if (holds[i] == CARD_EMPTY) {
            ++avail_holds;
        }
    }

    for (i = 0; i < COLUMN_COUNT; ++i) {
        if (column_counts[i] == 0 && i != dst_col) {
            ++avail_cols;
        }
    }

    return (1 + avail_holds) * (1 + avail_cols);
}

static int
get_move_count(int src_col, int dst_col)
{
    uint8_t dst_top, src_card;
    int max_seq_len, max_movable_count, n;

    if (column_counts[src_col] == 0 || column_counts[dst_col] == 0) {
        return 0;
    }

    dst_top = columns[dst_col][column_counts[dst_col] - 1];
    max_seq_len = get_max_valid_sequence_len(src_col);
    max_movable_count = get_max_movable_cards_count(dst_col);

    for (n = MIN(max_seq_len, max_movable_count); n >= 1; --n) {
        src_card = columns[src_col][column_counts[src_col] - n];

        if (card_rank(dst_top) == card_rank(src_card) + 1 &&
            card_color(dst_top) != card_color(src_card)) {
            return n;
        }
    }

    return 0;
}

static void
move_to_hold(void)
{
    uint8_t card;

    if (holds[cur_idx] != CARD_EMPTY) {
        show_error("Cell not empty");
        return;
    }

    card = lift_card();
    holds[cur_idx] = card;
    draw_cell(PILE_HOLDS, cur_idx);
    update_status();
}

static void
move_to_found(void)
{
    int suit, expected_rank;
    uint8_t card, top;

    if (sel_pile == PILE_NONE) {
        select_card();
    }

    if (sel_pile == PILE_NONE) {
        return;
    }

    card = selected_card();
    suit = card_suit(card);
    top = founds[suit];
    expected_rank = (top == CARD_EMPTY) ? 0 : card_rank(top) + 1;

    if (card_rank(card) != expected_rank) {
        show_error("Invalid move");
        return;
    }

    card = lift_card();
    founds[suit] = card;
    draw_cell(PILE_FOUNDS, suit);
    check_win();
    update_status();
}

static void
move_to_nonempty_col(void)
{
    int dst, src, n, i;
    uint8_t dst_top, src_card;

    dst = cur_idx;

    if (sel_pile == PILE_HOLDS) {
        dst_top = columns[dst][column_counts[dst] - 1];
        src_card = holds[sel_idx];

        if (card_rank(dst_top) != card_rank(src_card) + 1 ||
            card_color(dst_top) == card_color(src_card)) {
            show_error("Invalid move");
            return;
        }

        src_card = lift_card();
        columns[dst][column_counts[dst]++] = src_card;
        draw_column(dst);
        update_status();
    } else if (sel_pile == PILE_COLUMNS) {
        src = sel_idx;
        n = get_move_count(src, dst);
        if (n == 0) {
            show_error("Invalid move");
            return;
        }

        for (i = 0; i < n; ++i) {
            columns[dst][column_counts[dst]++] =
                columns[src][column_counts[src] - n + i];
        }

        column_counts[src] -= n;
        sel_pile = PILE_NONE;
        draw_column(src);
        draw_column(dst);
        update_status();
    }
}

static void
finish_move_to_empty_col(int dst, int count)
{
    int src, i;

    src = sel_idx;

    for (i = 0; i < count; ++i) {
        columns[dst][column_counts[dst]++] =
            columns[src][column_counts[src] - count + i];
    }

    column_counts[src] -= count;
    sel_pile = PILE_NONE;
    draw_column(src);
    draw_column(dst);
    update_status();
}

static void
start_move_to_empty_col(void)
{
    int max_seq_len, max_movable;
    uint8_t card;

    if (sel_pile == PILE_HOLDS) {
        card = lift_card();
        columns[cur_idx][column_counts[cur_idx]++] = card;
        draw_column(cur_idx);
        update_status();
        return;
    }

    max_seq_len = get_max_valid_sequence_len(sel_idx);
    max_movable = MIN(max_seq_len, get_max_movable_cards_count(cur_idx));

    if (max_movable <= 1) {
        finish_move_to_empty_col(cur_idx, 1);
        return;
    }

    state = STATE_ENTER_MOVE_COUNT;
    gui_status_set("How many? (0=max, 1-%d)", max_movable);
}

static void
handle_move_count(int key_ch)
{
    int count, max_seq_len, max_movable;

    state = STATE_DEFAULT;

    if (key_ch < '0' || key_ch > '9') {
        deselect_card();
        return;
    }

    count = key_ch - '0';
    max_seq_len = get_max_valid_sequence_len(sel_idx);
    max_movable = MIN(max_seq_len, get_max_movable_cards_count(cur_idx));

    if (count == 0) {
        count = max_movable;
    }

    if (count > max_movable) {
        show_error("Too many cards");
        return;
    }

    finish_move_to_empty_col(cur_idx, count);
}

static void
move_card(void)
{
    if (sel_pile == cur_pile && sel_idx == cur_idx) {
        deselect_card();
    } else if (cur_pile == PILE_HOLDS) {
        move_to_hold();
    } else if (column_counts[cur_idx] == 0) {
        start_move_to_empty_col();
    } else {
        move_to_nonempty_col();
    }
}

static void
handle_space(void)
{
    if (sel_pile == PILE_NONE) {
        select_card();
    } else {
        move_card();
    }
}

static void
show_help(void)
{
    rect_st r;
    int i, y;

    gui_rect_init(&r, HELP_X, HELP_Y, HELP_WIDTH, HELP_HEIGHT);
    gui_surface_draw_rect(&window.origin, &r, COLOR_BG);
    gui_surface_draw_border(&window.origin, &r, COLOR_FG);

    y = HELP_Y + HELP_PADDING;
    for (i = 0; i < HELP_LINE_COUNT; ++i) {
        gui_surface_draw_str(&window.origin, HELP_X + HELP_PADDING, y,
            NULL, help_lines[i], COLOR_FG, COLOR_BG);
        y += HELP_LINE_HEIGHT;
    }

    gui_surface_mark_dirty(&window.origin, &r);

    state = STATE_HELP;
}

static void
close_help(void)
{
    rect_st r;

    gui_rect_init(&r, HELP_X, HELP_Y, HELP_WIDTH, HELP_HEIGHT);
    gui_surface_draw_rect(&window.origin, &r, COLOR_BG);

    draw_piles();
    draw_cursor(1);

    state = STATE_DEFAULT;

    update_status();
}

static void
restart_game(void)
{
    draw_cursor(0);
    deal_cards();
    draw_piles();
    draw_cursor(1);
    update_status();
}

static void
on_key_down(window_st *win, const event_st *event)
{
    int key_code, key_ch;

    key_code = event->payload.key.key_code;
    key_ch = event->payload.key.key_char;

    if (state == STATE_HELP) {
        close_help();
        return;
    }

    if (state == STATE_ENTER_MOVE_COUNT) {
        handle_move_count(key_ch);
        return;
    }

    if (state == STATE_WON) {
        if (key_ch == 'r') {
            restart_game();
        }
        return;
    }

    switch (key_code) {
        case KEY_LEFT:  move_cursor(-1, 0); return;
        case KEY_RIGHT: move_cursor(1, 0); return;
        case KEY_UP:    move_cursor(0, -1); return;
        case KEY_DOWN:  move_cursor(0, 1); return;
        case KEY_SPACE: handle_space(); return;
    }

    if (key_ch == 'f') {
        move_to_found();
    } else if (key_ch == 'r') {
        restart_game();
    } else if (key_ch == 'h') {
        show_help();
    }
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);
    window.bg_color = COLOR_BG;
    window.on_key_down = on_key_down;
}

static void
on_show(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_window();
        initialized = 1;
    }


    restart_game();
}

global app_st app_freecell = {
    "FreeCell",
    &icon_fcell,
    &window,
    on_show,
};
