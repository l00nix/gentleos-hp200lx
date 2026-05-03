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

    WINDOW_WIDTH = COLUMN_COUNT * CARD_WIDTH + (COLUMN_COUNT + 1) * GAP_X,
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
    STATE_AUTO_PENDING = 4,

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

typedef struct {
    int src_pile;
    int src_idx;
    int dst_pile;
    int dst_idx;
    int count;
} card_move_st;

static window_st window;

static uint8_t holds[HOLD_COUNT];
static uint8_t founds[FOUND_COUNT];
static uint8_t columns[COLUMN_COUNT][COLUMN_CARDS_MAX];
static int column_counts[COLUMN_COUNT];

static card_move_st cur_move = { PILE_NONE, 0, PILE_NONE, 0, 0 };

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
    return col * (CARD_WIDTH + GAP_X) + GAP_X;
}

static int
col_step(int col)
{
    int step, max_step;
    int count = column_counts[col];

    step = COLUMN_CARDS_STEP;
    if (count > 1) {
        max_step = (COLUMNS_H - CARD_HEIGHT) / (count - 1);
        step = MIN(step, max_step);
        step = MAX(step, 1);
    }

    return step;
}

static int
pile_x(int pile, int idx)
{
    switch (pile) {
    case PILE_COLUMNS: return col_x(idx);
    case PILE_HOLDS: return col_x(idx) - GAP_X;
    case PILE_FOUNDS: return col_x(idx + HOLD_COUNT) + GAP_X;
    default: return 0;
    }
}

static uint8_t
pile_top_card(int pile, int idx)
{
    int col_count = column_counts[idx];

    switch (pile) {
    case PILE_COLUMNS: return col_count > 0 ? columns[idx][col_count - 1] : CARD_EMPTY;
    case PILE_HOLDS: return holds[idx];
    case PILE_FOUNDS: return founds[idx];
    default: return CARD_EMPTY;
    }
}

static int
pile_top_card_y(int pile, int idx)
{
    int col_count = column_counts[idx];

    switch (pile) {
    case PILE_COLUMNS: return COLUMNS_Y + (col_count > 0 ? (col_count - 1) * col_step(idx) : 0);
    case PILE_HOLDS:
    case PILE_FOUNDS: return HOLDS_Y;
    default: return 0;
    }
}

static int
pile_is_sel(int pile, int idx)
{
    return cur_move.src_pile == pile && cur_move.src_idx == idx;
}

static uint8_t
selected_card(void)
{
    return pile_top_card(cur_move.src_pile, cur_move.src_idx);
}

static int
remaining_cards(void)
{
    int i;
    int ret = 0;

    for (i = 0; i < COLUMN_COUNT; ++i) {
        ret += column_counts[i];
    }

    for (i = 0; i < HOLD_COUNT; ++i) {
        ret += holds[i] == CARD_EMPTY ? 0 : 1;
    }

    return ret;
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

    cur_move.src_pile = PILE_NONE;
    cur_move.src_idx = 0;

    cur_pile = PILE_COLUMNS;
    cur_idx = 0;

    state = STATE_DEFAULT;
}

static void
draw_card(int x, int y, uint8_t card, int is_sel)
{
    uint8_t fg = is_sel ? gui_color_bg : gui_color_fg;
    uint8_t bg = is_sel ? gui_color_fg : gui_color_bg;
    int rank = card_rank(card);
    int suit = card_suit(card);
    rect_st r;

    gui_rect_init(&r, x, y, CARD_WIDTH, CARD_HEIGHT);
    gui_surface_draw_rect(&window.origin, &r, bg);
    gui_surface_draw_border(&window.origin, &r, gui_color_fg);
    gui_surface_mark_dirty(&window.origin, &r);

    if (card == CARD_EMPTY) {
        return;
    }

    gui_surface_draw_str(&window.origin, x + 3, y + 3, &fonts[2], rank_str[rank], fg, bg);
    gui_surface_draw_bitmap(&window.origin, &window.size,
        x + CARD_WIDTH - 8, y + 3, suit_bmp[suit], fg);
}

static void
draw_card_stub(int x, int y, int height, uint8_t card)
{
    int rank = card_rank(card);
    int suit = card_suit(card);
    rect_st r;

    gui_rect_init(&r, x, y, CARD_WIDTH, height);
    gui_surface_draw_rect(&window.origin, &r, gui_color_bg);
    gui_surface_draw_border(&window.origin, &r, gui_color_fg);

    gui_surface_draw_str(&window.origin, x + 3, y + 3, &fonts[2],
        rank_str[rank], gui_color_fg, gui_color_bg);
    gui_surface_draw_bitmap(&window.origin, &window.size,
        x + CARD_WIDTH - 8, y + 3, suit_bmp[suit], gui_color_fg);

    gui_surface_mark_dirty(&window.origin, &r);
}

static void
draw_cursor(int visible)
{
    int x = pile_x(cur_pile, cur_idx);
    int y = pile_top_card_y(cur_pile, cur_idx);
    int is_sel = pile_is_sel(cur_pile, cur_idx);
    uint8_t color = (visible ^ is_sel) ? gui_color_fg : gui_color_bg;
    rect_st r;

    gui_rect_init(&r, x + 1, y + 1, CARD_WIDTH - 2, CARD_HEIGHT - 2);
    gui_surface_draw_border(&window.origin, &r, color);
    gui_surface_mark_dirty(&window.origin, &r);
}

static void
draw_cell(int pile, int idx)
{
    int x = pile_x(pile, idx);
    int y = HOLDS_Y;
    int is_sel = pile_is_sel(pile, idx);
    uint8_t card = pile_top_card(pile, idx);
    rect_st r;

    draw_card(x, y, card, is_sel);

    if (cur_pile == pile && cur_idx == idx) {
        draw_cursor(1);
    }

    gui_rect_init(&r, x, y, CARD_WIDTH, CARD_HEIGHT);
    gui_surface_mark_dirty(&window.origin, &r);
}

static void
draw_column(int col)
{
    int x = col_x(col);
    int count = column_counts[col];
    uint8_t top_card = pile_top_card(PILE_COLUMNS, col);
    int top_card_y = pile_top_card_y(PILE_COLUMNS, col);
    int is_sel = top_card != CARD_EMPTY && pile_is_sel(PILE_COLUMNS, col);

    int i, step;
    rect_st r;

    gui_rect_init(&r, x, COLUMNS_Y, CARD_WIDTH, COLUMNS_H);
    gui_surface_draw_rect(&window.origin, &r, gui_color_bg);

    if (count > 0) {
        step = col_step(col);

        for (i = 0; i < count - 1; i++) {
            draw_card_stub(x, COLUMNS_Y + i * step, step + 1, columns[col][i]);
        }
    }

    draw_card(x, top_card_y, top_card, is_sel);

    if (cur_pile == PILE_COLUMNS && cur_idx == col) {
        draw_cursor(1);
    }

    gui_surface_mark_dirty(&window.origin, &r);
}

static void
draw_pile(int pile, int idx)
{
    if (pile == PILE_COLUMNS) {
        draw_column(idx);
    } else {
        draw_cell(pile, idx);
    }
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
move_cursor(int dx, int dy)
{
    int max_idx;

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
    int remaining = remaining_cards();

    if (state == STATE_WON) {
        gui_status_set("You Won! Press R to restart");
    } else {
        gui_status_set("Remaining cards: %d", remaining);
        gui_status_set_br("H: Show controls");
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
    update_status();
}

static void
start_move(void)
{
    if (cur_pile == PILE_HOLDS && holds[cur_idx] != CARD_EMPTY) {
        cur_move.src_pile = PILE_HOLDS;
        cur_move.src_idx = cur_idx;
        draw_cell(PILE_HOLDS, cur_idx);
        update_status();
    } else if (cur_pile == PILE_COLUMNS && column_counts[cur_idx] > 0) {
        cur_move.src_pile = PILE_COLUMNS;
        cur_move.src_idx = cur_idx;
        draw_column(cur_idx);
        update_status();
    }
}

static void
cancel_move(void)
{
    int old_pile = cur_move.src_pile;
    int old_idx = cur_move.src_idx;

    cur_move.src_pile = PILE_NONE;

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
    cancel_move();
    gui_status_set("%s", msg);
}

static int
card_should_auto_promote(uint8_t card)
{
    int rank = card_rank(card);
    int suit = card_suit(card);
    int color = card_color(card);
    int i;

    if (founds[suit] == CARD_EMPTY) {
        if (rank != 0) {
            return 0;
        }
    } else if (rank != card_rank(founds[suit]) + 1) {
        return 0;
    }

    if (rank <= 1) {
        return 1;
    }

    for (i = 0; i < FOUND_COUNT; ++i) {
        if (card_color(i * 13) == color) {
            continue;
        }

        if (founds[i] == CARD_EMPTY || card_rank(founds[i]) < rank - 1) {
            return 0;
        }
    }

    return 1;
}

static void
check_auto_move(void)
{
    int i;
    uint8_t card;

    for (i = 0; i < HOLD_COUNT; ++i) {
        card = holds[i];

        if (card == CARD_EMPTY) {
            continue;
        }

        if (card_should_auto_promote(card)) {
            cur_move.src_pile = PILE_HOLDS;
            cur_move.src_idx = i;
            cur_move.dst_pile = PILE_FOUNDS;
            cur_move.dst_idx = card_suit(card);
            cur_move.count = 1;
            state = STATE_AUTO_PENDING;
            return;
        }
    }

    for (i = 0; i < COLUMN_COUNT; ++i) {
        if (column_counts[i] == 0) {
            continue;
        }

        card = columns[i][column_counts[i] - 1];

        if (card_should_auto_promote(card)) {
            cur_move.src_pile = PILE_COLUMNS;
            cur_move.src_idx = i;
            cur_move.dst_pile = PILE_FOUNDS;
            cur_move.dst_idx = card_suit(card);
            cur_move.count = 1;
            state = STATE_AUTO_PENDING;
            return;
        }
    }
}

static void
exec_move(void)
{
    int i;
    uint8_t card;
    int src_pile = cur_move.src_pile;
    int src_idx = cur_move.src_idx;
    int dst_pile = cur_move.dst_pile;
    int dst_idx = cur_move.dst_idx;
    int count = cur_move.count;

    cur_move.src_pile = PILE_NONE;

    ASSERT(src_pile != PILE_NONE);
    ASSERT(dst_pile != PILE_NONE);

    if (src_pile == PILE_COLUMNS && dst_pile == PILE_COLUMNS) {
        ASSERT(count <= column_counts[src_idx]);

        for (i = 0; i < count; ++i) {
            columns[dst_idx][column_counts[dst_idx]++] =
                columns[src_idx][column_counts[src_idx] - count + i];
        }

        column_counts[src_idx] -= count;
    } else {
        ASSERT(count == 1);

        if (src_pile == PILE_HOLDS) {
            card = holds[src_idx];
            ASSERT(card != CARD_EMPTY);
            holds[src_idx] = CARD_EMPTY;
        } else if (src_pile == PILE_FOUNDS) {
            card = founds[src_idx];
            ASSERT(card != CARD_EMPTY);
            founds[src_idx] = CARD_EMPTY;
        } else if (src_pile == PILE_COLUMNS) {
            ASSERT(column_counts[src_idx] > 0);
            card = columns[src_idx][--column_counts[src_idx]];
        }

        if (dst_pile == PILE_HOLDS) {
            ASSERT(holds[dst_idx] == CARD_EMPTY);
            holds[dst_idx] = card;
        } else if (dst_pile == PILE_FOUNDS) {
            founds[dst_idx] = card;
        } else if (dst_pile == PILE_COLUMNS) {
            columns[dst_idx][column_counts[dst_idx]++] = card;
        }
    }

    draw_pile(src_pile, src_idx);
    draw_pile(dst_pile, dst_idx);

    update_status();
    check_win();
    check_auto_move();
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
request_move_to_hold(void)
{
    if (holds[cur_move.dst_idx] != CARD_EMPTY) {
        show_error("Cell not empty");
        return;
    }

    cur_move.count = 1;
    exec_move();
}

static void
request_move_to_found(void)
{
    int expected_rank;
    uint8_t card, suit, top;

    if (cur_move.src_pile == PILE_NONE) {
        start_move();
    }

    if (cur_move.src_pile == PILE_NONE) {
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

    cur_move.dst_pile = PILE_FOUNDS;
    cur_move.dst_idx = suit;
    cur_move.count = 1;
    exec_move();
}

static void
request_move_to_nonempty_col(void)
{
    uint8_t dst_top, src_card;

    if (cur_move.src_pile == PILE_HOLDS) {
        src_card = holds[cur_move.src_idx];
        dst_top = columns[cur_move.dst_idx][column_counts[cur_move.dst_idx] - 1];

        if (card_rank(dst_top) != card_rank(src_card) + 1 ||
            card_color(dst_top) == card_color(src_card)) {
            show_error("Invalid move");
            return;
        }

        cur_move.count = 1;
    } else if (cur_move.src_pile == PILE_COLUMNS) {
        cur_move.count = get_move_count(cur_move.src_idx, cur_move.dst_idx);

        if (cur_move.count == 0) {
            show_error("Invalid move");
            return;
        }
    }

    exec_move();
}

static void
request_move_to_empty_col(void)
{
    int max_seq_len, max_movable;

    if (cur_move.src_pile == PILE_HOLDS) {
        cur_move.count = 1;
        exec_move();
        return;
    }

    ASSERT(cur_move.src_pile == PILE_COLUMNS);

    max_seq_len = get_max_valid_sequence_len(cur_move.src_idx);
    max_movable = MIN(max_seq_len, get_max_movable_cards_count(cur_move.dst_idx));

    if (max_movable <= 1) {
        cur_move.count = 1;
        exec_move();
        return;
    }

    state = STATE_ENTER_MOVE_COUNT;
    gui_status_set("How many? (0=max, 1-%d)", max_movable);
}

static void
handle_move_count(int key_code)
{
    int count, max_seq_len, max_movable;

    state = STATE_DEFAULT;

    count = key_number_for_code(key_code);

    if (count < 0) {
        cancel_move();
        return;
    }

    max_seq_len = get_max_valid_sequence_len(cur_move.src_idx);
    max_movable = MIN(max_seq_len, get_max_movable_cards_count(cur_move.dst_idx));

    if (count == 0) {
        count = max_movable;
    }

    if (count > max_movable) {
        show_error("Too many cards");
        return;
    }

    cur_move.count = count;
    exec_move();
}

static void
request_move(void)
{
    cur_move.dst_pile = cur_pile;
    cur_move.dst_idx = cur_idx;

    if (cur_move.dst_pile == PILE_HOLDS) {
        request_move_to_hold();
    } else if (column_counts[cur_move.dst_idx] == 0) {
        request_move_to_empty_col();
    } else {
        request_move_to_nonempty_col();
    }
}

static void
handle_space(void)
{
    if (cur_move.src_pile == PILE_NONE) {
        start_move();
    } else if (cur_move.src_pile == cur_pile && cur_move.src_idx == cur_idx) {
        cancel_move();
    } else {
        request_move();
    }
}

static void
show_help(void)
{
    rect_st r;
    int i, y;

    gui_rect_init(&r, HELP_X, HELP_Y, HELP_WIDTH, HELP_HEIGHT);
    gui_surface_draw_rect(&window.origin, &r, gui_color_bg);
    gui_surface_draw_border(&window.origin, &r, gui_color_fg);

    y = HELP_Y + HELP_PADDING;
    for (i = 0; i < HELP_LINE_COUNT; ++i) {
        gui_surface_draw_str(&window.origin, HELP_X + HELP_PADDING, y,
            NULL, help_lines[i], gui_color_fg, gui_color_bg);
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
    gui_surface_draw_rect(&window.origin, &r, gui_color_bg);

    draw_piles();
    draw_cursor(1);

    state = STATE_DEFAULT;

    update_status();
}

static void
restart_game(void)
{
    deal_cards();
    draw_piles();
    draw_cursor(1);
    update_status();
}

static void
on_tick(void)
{
    static int ticks_waited = 0;

    if (state != STATE_AUTO_PENDING) {
        ticks_waited = 0;
        return;
    }

    ++ticks_waited;

    if (ticks_waited == 2) {
        draw_pile(cur_move.src_pile, cur_move.src_idx);
        return;
    }

    if (ticks_waited > 5) {
        ticks_waited = 0;
        state = STATE_DEFAULT;
        exec_move();
    }
}

static void
on_key_down(uint8_t key_code, uint8_t key_mods)
{
    if (state == STATE_AUTO_PENDING) {
        return;
    }

    if (state == STATE_HELP) {
        close_help();
        return;
    }

    if (state == STATE_ENTER_MOVE_COUNT) {
        handle_move_count(key_code);
        return;
    }

    if (state == STATE_WON) {
        if (key_code == KEY_R) {
            restart_game();
        }

        return;
    }

    switch (key_code) {
        case KEY_LEFT: move_cursor(-1, 0); return;
        case KEY_RIGHT: move_cursor(1, 0); return;
        case KEY_UP: move_cursor(0, -1); return;
        case KEY_DOWN: move_cursor(0, 1); return;
        case KEY_SPACE: handle_space(); return;
        case KEY_F: request_move_to_found(); return;
        case KEY_R: restart_game(); return;
        case KEY_H: show_help(); return;
    }
}

static void
on_show(void)
{
    static int initialized = 0;

    if (!initialized) {
        gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

        app_freecell.on_key_down = on_key_down;
        app_freecell.on_tick = on_tick;

        initialized = 1;
    }

    restart_game();
}

global app_st app_freecell = {
    "FreeCell",
    &icon_fcell,
    on_show,
};
