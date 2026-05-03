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

typedef uint8_t card_t;

typedef struct {
    uint8_t type;
    int index;
    rect_st rect;
    uint8_t capacity;
    uint8_t count;
    card_t *cards;
    unsigned is_cascade : 1;
    unsigned replace_on_push : 1;
} card_pile_st;

typedef struct {
    card_pile_st *src;
    card_pile_st *dst;
    int count;
} card_move_st;

static window_st window;

static card_t holds_cards[HOLD_COUNT];
static card_pile_st holds[HOLD_COUNT];

static card_t founds_cards[FOUND_COUNT];
static card_pile_st founds[FOUND_COUNT];

static card_t columns_cards[COLUMN_COUNT][COLUMN_CARDS_MAX];
static card_pile_st columns[COLUMN_COUNT];

static card_move_st cur_move = { NULL, NULL, 0 };
static card_pile_st *cur_pile;

static int state;

static int
card_rank(card_t card)
{
    return card % 13;
}

static int
card_suit(card_t card)
{
    return card / 13;
}

static int
card_color(card_t card)
{
    return card_suit(card) / 2;
}

static card_pile_st *
get_pile(int type, int idx)
{
    switch (type) {
    case PILE_HOLDS: return &holds[idx];
    case PILE_FOUNDS: return &founds[idx];
    case PILE_COLUMNS: return &columns[idx];
    default: return NULL;
    }
}

static card_t
pile_top(card_pile_st *p)
{
    return p->count > 0 ? p->cards[p->count - 1] : CARD_EMPTY;
}

static card_t
pile_pop(card_pile_st *p)
{
    ASSERT(p->count > 0);
    return p->cards[--p->count];
}

static void
pile_push(card_pile_st *p, card_t c)
{
    if (p->replace_on_push) {
        p->cards[0] = c;
        p->count = 1;
        return;
    }

    ASSERT(p->count < p->capacity);
    p->cards[p->count++] = c;
}

static int
pile_is_sel(card_pile_st *p)
{
    return cur_move.src == p;
}

static int
pile_cascade_step(card_pile_st *p)
{
    int step, max_step;

    step = COLUMN_CARDS_STEP;
    if (p->count > 1) {
        max_step = (p->rect.height - CARD_HEIGHT) / (p->count - 1);
        step = MIN(step, max_step);
        step = MAX(step, 1);
    }

    return step;
}

static int
pile_top_y(card_pile_st *p)
{
    if (!p->is_cascade || p->count <= 1) {
        return p->rect.y;
    }

    return p->rect.y + (p->count - 1) * pile_cascade_step(p);
}

static card_t
selected_card(void)
{
    return pile_top(cur_move.src);
}

static int
remaining_cards(void)
{
    int i;
    int ret = 0;

    for (i = 0; i < HOLD_COUNT; ++i) {
        ret += holds[i].count;
    }

    for (i = 0; i < COLUMN_COUNT; ++i) {
        ret += columns[i].count;
    }

    return ret;
}

static void
deal_cards(void)
{
    card_t deck[CARD_COUNT];
    int i, j, col;
    card_t tmp;

    for (i = 0; i < CARD_COUNT; ++i) {
        deck[i] = i;
    }

    for (i = CARD_COUNT - 1; i > 0; --i) {
        j = rand() % (i + 1);
        tmp = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp;
    }

    for (i = 0; i < COLUMN_COUNT; ++i) {
        columns[i].count = 0;
    }

    for (i = 0; i < HOLD_COUNT; ++i) {
        holds[i].count = 0;
    }

    for (i = 0; i < FOUND_COUNT; ++i) {
        founds[i].count = 0;
    }

    for (i = 0; i < CARD_COUNT; ++i) {
        col = i % COLUMN_COUNT;
        pile_push(&columns[col], deck[i]);
    }

    cur_move.src = NULL;
    cur_pile = &columns[0];

    state = STATE_DEFAULT;
}

static void
draw_card(int x, int y, card_t card, int is_sel)
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
draw_card_stub(int x, int y, int height, card_t card)
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
    int x = cur_pile->rect.x;
    int y = pile_top_y(cur_pile);
    int is_sel = pile_is_sel(cur_pile);
    uint8_t color = (visible ^ is_sel) ? gui_color_fg : gui_color_bg;
    rect_st r;

    gui_rect_init(&r, x + 1, y + 1, CARD_WIDTH - 2, CARD_HEIGHT - 2);
    gui_surface_draw_border(&window.origin, &r, color);
    gui_surface_mark_dirty(&window.origin, &r);
}

static void
draw_pile(card_pile_st *p)
{
    int x = p->rect.x;
    int top_y = pile_top_y(p);
    card_t top = pile_top(p);
    int is_sel = top != CARD_EMPTY && pile_is_sel(p);
    int i, step;

    gui_surface_draw_rect(&window.origin, &p->rect, gui_color_bg);

    if (p->is_cascade && p->count > 1) {
        step = pile_cascade_step(p);
        for (i = 0; i < p->count - 1; ++i) {
            draw_card_stub(x, p->rect.y + i * step, step + 1, p->cards[i]);
        }
    }

    draw_card(x, top_y, top, is_sel);

    if (cur_pile == p) {
        draw_cursor(1);
    }

    gui_surface_mark_dirty(&window.origin, &p->rect);
}

static void
draw_piles(void)
{
    int i;

    for (i = 0; i < HOLD_COUNT; ++i) {
        draw_pile(&holds[i]);
    }

    for (i = 0; i < FOUND_COUNT; ++i) {
        draw_pile(&founds[i]);
    }

    for (i = 0; i < COLUMN_COUNT; ++i) {
        draw_pile(&columns[i]);
    }
}

static void
move_cursor(int dx, int dy)
{
    int new_type, new_idx, max_idx;

    draw_cursor(0);

    if (dy < 0) {
        new_type = PILE_HOLDS;
    } else if (dy > 0) {
        new_type = PILE_COLUMNS;
    } else {
        new_type = cur_pile->type;
    }

    max_idx = (new_type == PILE_COLUMNS ? COLUMN_COUNT :  HOLD_COUNT) - 1;

    new_idx = cur_pile->index + dx;
    new_idx = MAX(0, new_idx);
    new_idx = MIN(new_idx, max_idx);

    cur_pile = get_pile(new_type, new_idx);

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
    card_pile_st *pile;

    for (i = 0; i < FOUND_COUNT; ++i) {
        if (founds[i].count == 0 || card_rank(pile_top(&founds[i])) != 12) {
            return;
        }
    }

    state = STATE_WON;
    update_status();
}

static void
start_move(void)
{
    if (cur_pile->type != PILE_HOLDS && cur_pile->type != PILE_COLUMNS) {
        return;
    }

    if (cur_pile->count == 0) {
        return;
    }

    cur_move.src = cur_pile;
    draw_pile(cur_pile);
    update_status();
}

static void
cancel_move(void)
{
    card_pile_st *old = cur_move.src;

    cur_move.src = NULL;

    if (old != NULL) {
        draw_pile(old);
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
card_should_auto_promote(card_t card)
{
    int rank = card_rank(card);
    int suit = card_suit(card);
    int color = card_color(card);
    int i;

    if (founds[suit].count == 0) {
        if (rank != 0) {
            return 0;
        }
    } else if (rank != card_rank(pile_top(&founds[suit])) + 1) {
        return 0;
    }

    if (rank <= 1) {
        return 1;
    }

    for (i = 0; i < FOUND_COUNT; ++i) {
        if (card_color(i * 13) == color) {
            continue;
        }

        if (founds[i].count == 0 || card_rank(pile_top(&founds[i])) < rank - 1) {
            return 0;
        }
    }

    return 1;
}

static void
set_auto_move(card_pile_st *src, card_pile_st *dst)
{
    cur_move.src = src;
    cur_move.dst = dst;
    cur_move.count = 1;
    state = STATE_AUTO_PENDING;
}

static void
check_auto_move(void)
{
    int i;
    card_t card;

    for (i = 0; i < HOLD_COUNT; ++i) {
        card = pile_top(&holds[i]);

        if (card != CARD_EMPTY && card_should_auto_promote(card)) {
            set_auto_move(&holds[i], &founds[card_suit(card)]);
            return;
        }
    }

    for (i = 0; i < COLUMN_COUNT; ++i) {
        card = pile_top(&columns[i]);

        if (card != CARD_EMPTY && card_should_auto_promote(card)) {
            set_auto_move(&columns[i], &founds[card_suit(card)]);
            return;
        }
    }
}

static void
exec_move(void)
{
    int i;
    int count = cur_move.count;
    card_pile_st *src = cur_move.src;
    card_pile_st *dst = cur_move.dst;

    cur_move.src = NULL;

    ASSERT(src != NULL);
    ASSERT(dst != NULL);

    if (src->type == PILE_COLUMNS && dst->type == PILE_COLUMNS) {
        ASSERT(count <= src->count);

        for (i = 0; i < count; ++i) {
            pile_push(dst, src->cards[src->count - count + i]);
        }

        src->count -= count;
    } else {
        ASSERT(count == 1);
        pile_push(dst, pile_pop(src));
    }

    draw_pile(src);
    draw_pile(dst);

    update_status();
    check_win();
    check_auto_move();
}

static int
get_max_valid_sequence_len(card_pile_st *p)
{
    int count, i;
    card_t curr, prev;

    count = p->count;

    if (count == 0) {
        return 0;
    }

    for (i = count - 1; i > 0; --i) {
        curr = p->cards[i];
        prev = p->cards[i - 1];

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
get_max_movable_cards_count(card_pile_st *dst)
{
    int i;
    int avail_holds = 0;
    int avail_cols = 0;
    card_pile_st *col;

    for (i = 0; i < HOLD_COUNT; ++i) {
        if (holds[i].count == 0) {
            ++avail_holds;
        }
    }

    for (i = 0; i < COLUMN_COUNT; ++i) {
        if (columns[i].count == 0 && dst != &columns[i]) {
            ++avail_cols;
        }
    }

    return (1 + avail_holds) * (1 + avail_cols);
}

static int
get_move_count(card_pile_st *src, card_pile_st *dst)
{
    card_t dst_top, src_card;
    int max_seq_len, max_movable_count, n;

    if (src->count == 0 || dst->count == 0) {
        return 0;
    }

    dst_top = pile_top(dst);
    max_seq_len = get_max_valid_sequence_len(src);
    max_movable_count = get_max_movable_cards_count(dst);

    for (n = MIN(max_seq_len, max_movable_count); n >= 1; --n) {
        src_card = src->cards[src->count - n];

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
    if (cur_move.dst->count > 0) {
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
    card_t card;
    card_pile_st *found;

    if (cur_move.src == NULL) {
        start_move();
    }

    if (cur_move.src == NULL) {
        return;
    }

    card = selected_card();
    found = &founds[card_suit(card)];
    expected_rank = (found->count == 0) ? 0 : card_rank(pile_top(found)) + 1;

    if (card_rank(card) != expected_rank) {
        show_error("Invalid move");
        return;
    }

    cur_move.dst = found;
    cur_move.count = 1;
    exec_move();
}

static void
request_move_to_nonempty_col(void)
{
    card_t dst_top, src_card;

    if (cur_move.src->type == PILE_HOLDS) {
        src_card = pile_top(cur_move.src);
        dst_top = pile_top(cur_move.dst);

        if (card_rank(dst_top) != card_rank(src_card) + 1 ||
            card_color(dst_top) == card_color(src_card)) {
            show_error("Invalid move");
            return;
        }

        cur_move.count = 1;
    } else if (cur_move.src->type == PILE_COLUMNS) {
        cur_move.count = get_move_count(cur_move.src, cur_move.dst);

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

    if (cur_move.src->type == PILE_HOLDS) {
        cur_move.count = 1;
        exec_move();
        return;
    }

    ASSERT(cur_move.src->type == PILE_COLUMNS);

    max_seq_len = get_max_valid_sequence_len(cur_move.src);
    max_movable = MIN(max_seq_len, get_max_movable_cards_count(cur_move.dst));

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

    max_seq_len = get_max_valid_sequence_len(cur_move.src);
    max_movable = MIN(max_seq_len, get_max_movable_cards_count(cur_move.dst));

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
    cur_move.dst = cur_pile;

    if (cur_pile->type == PILE_HOLDS) {
        request_move_to_hold();
    } else if (cur_pile->count == 0) {
        request_move_to_empty_col();
    } else {
        request_move_to_nonempty_col();
    }
}

static void
handle_space(void)
{
    if (cur_move.src == NULL) {
        start_move();
    } else if (cur_move.src == cur_pile) {
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
        draw_pile(cur_move.src);
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
init_piles(void)
{
    int i;

    for (i = 0; i < HOLD_COUNT; ++i) {
        holds[i].type = PILE_HOLDS;
        holds[i].index = i;
        holds[i].capacity = 1;
        holds[i].count = 0;
        holds[i].cards = &holds_cards[i];
        holds[i].is_cascade = 0;
        holds[i].replace_on_push = 0;

        gui_rect_init(&holds[i].rect,
            i * (CARD_WIDTH + GAP_X), HOLDS_Y, CARD_WIDTH, CARD_HEIGHT);
    }

    for (i = 0; i < FOUND_COUNT; ++i) {
        founds[i].type = PILE_FOUNDS;
        founds[i].index = i;
        founds[i].capacity = 1;
        founds[i].count = 0;
        founds[i].cards = &founds_cards[i];
        founds[i].is_cascade = 0;
        founds[i].replace_on_push = 1;

        gui_rect_init(&founds[i].rect,
            (i + HOLD_COUNT) * (CARD_WIDTH + GAP_X) + 2 * GAP_X, HOLDS_Y,
            CARD_WIDTH, CARD_HEIGHT);
    }

    for (i = 0; i < COLUMN_COUNT; ++i) {
        columns[i].type = PILE_COLUMNS;
        columns[i].index = i;
        columns[i].capacity = COLUMN_CARDS_MAX;
        columns[i].count = 0;
        columns[i].cards = columns_cards[i];
        columns[i].is_cascade = 1;
        columns[i].replace_on_push = 0;

        gui_rect_init(&columns[i].rect,
            i * (CARD_WIDTH + GAP_X) + GAP_X, COLUMNS_Y,
            CARD_WIDTH, COLUMNS_H);
    }
}

static void
on_show(void)
{
    static int initialized = 0;

    if (!initialized) {
        gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);
        init_piles();

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
