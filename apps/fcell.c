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

static const char *help_lines[] = {
    "Arrows: move cursor",
    "Space:  select / place",
    "F:      move to foundation",
    "R:      restart game",
    "H:      toggle help",
};

static window_st window;

static card_t holds_cards[HOLD_COUNT];
static card_pile_st holds[HOLD_COUNT];

static card_t founds_cards[FOUND_COUNT];
static card_pile_st founds[FOUND_COUNT];

static card_t columns_cards[COLUMN_COUNT][COLUMN_CARDS_MAX];
static card_pile_st columns[COLUMN_COUNT];

static card_game_st game;

static int state;

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
        card_pile_push(&columns[col], deck[i]);
    }

    game.cur_move.src = NULL;
    game.cur_pile = &columns[0];

    state = STATE_DEFAULT;
}


static void
draw_piles(void)
{
    int i;

    for (i = 0; i < HOLD_COUNT; ++i) {
        card_pile_draw(&game, &holds[i]);
    }

    for (i = 0; i < FOUND_COUNT; ++i) {
        card_pile_draw(&game, &founds[i]);
    }

    for (i = 0; i < COLUMN_COUNT; ++i) {
        card_pile_draw(&game, &columns[i]);
    }
}

static void
move_cursor(int dx, int dy)
{
    int new_type, new_idx, max_idx;

    card_cursor_draw(&game, 0);

    if (dy < 0) {
        new_type = PILE_HOLDS;
    } else if (dy > 0) {
        new_type = PILE_COLUMNS;
    } else {
        new_type = game.cur_pile->type;
    }

    max_idx = (new_type == PILE_COLUMNS ? COLUMN_COUNT :  HOLD_COUNT) - 1;

    new_idx = game.cur_pile->index + dx;
    new_idx = MAX(0, new_idx);
    new_idx = MIN(new_idx, max_idx);

    game.cur_pile = get_pile(new_type, new_idx);

    card_cursor_draw(&game, 1);
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
        if (founds[i].count == 0 || CARD_RANK(CARD_PILE_TOP(&founds[i])) != 12) {
            return;
        }
    }

    state = STATE_WON;
    update_status();
}

static void
start_move(void)
{
    if (game.cur_pile->type != PILE_HOLDS && game.cur_pile->type != PILE_COLUMNS) {
        return;
    }

    if (game.cur_pile->count == 0) {
        return;
    }

    game.cur_move.src = game.cur_pile;
    card_pile_draw(&game, game.cur_pile);
    update_status();
}

static void
cancel_move(void)
{
    card_pile_st *old = game.cur_move.src;

    game.cur_move.src = NULL;

    if (old != NULL) {
        card_pile_draw(&game, old);
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
    int rank = CARD_RANK(card);
    int suit = CARD_SUIT(card);
    int color = CARD_COLOR(card);
    int i;

    if (founds[suit].count == 0) {
        if (rank != 0) {
            return 0;
        }
    } else if (rank != CARD_RANK(CARD_PILE_TOP(&founds[suit])) + 1) {
        return 0;
    }

    if (rank <= 1) {
        return 1;
    }

    for (i = 0; i < FOUND_COUNT; ++i) {
        if (CARD_COLOR(i * 13) == color) {
            continue;
        }

        if (founds[i].count == 0 || CARD_RANK(CARD_PILE_TOP(&founds[i])) < rank - 1) {
            return 0;
        }
    }

    return 1;
}

static void
set_auto_move(card_pile_st *src, card_pile_st *dst)
{
    game.cur_move.src = src;
    game.cur_move.dst = dst;
    game.cur_move.count = 1;
    state = STATE_AUTO_PENDING;
}

static void
check_auto_move(void)
{
    int i;
    card_t card;

    for (i = 0; i < HOLD_COUNT; ++i) {
        card = CARD_PILE_TOP(&holds[i]);

        if (card != CARD_EMPTY && card_should_auto_promote(card)) {
            set_auto_move(&holds[i], &founds[CARD_SUIT(card)]);
            return;
        }
    }

    for (i = 0; i < COLUMN_COUNT; ++i) {
        card = CARD_PILE_TOP(&columns[i]);

        if (card != CARD_EMPTY && card_should_auto_promote(card)) {
            set_auto_move(&columns[i], &founds[CARD_SUIT(card)]);
            return;
        }
    }
}

static void
exec_move(void)
{
    int i;
    int count = game.cur_move.count;
    card_pile_st *src = game.cur_move.src;
    card_pile_st *dst = game.cur_move.dst;

    game.cur_move.src = NULL;

    ASSERT(src != NULL);
    ASSERT(dst != NULL);

    if (src->type == PILE_COLUMNS && dst->type == PILE_COLUMNS) {
        ASSERT(count <= src->count);

        for (i = 0; i < count; ++i) {
            card_pile_push(dst, src->cards[src->count - count + i]);
        }

        src->count -= count;
    } else {
        ASSERT(count == 1);
        card_pile_push(dst, card_pile_pop(src));
    }

    card_pile_draw(&game, src);
    card_pile_draw(&game, dst);

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

        if (CARD_RANK(prev) != CARD_RANK(curr) + 1) {
            break;
        }

        if (CARD_COLOR(prev) == CARD_COLOR(curr)) {
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

    dst_top = CARD_PILE_TOP(dst);
    max_seq_len = get_max_valid_sequence_len(src);
    max_movable_count = get_max_movable_cards_count(dst);

    for (n = MIN(max_seq_len, max_movable_count); n >= 1; --n) {
        src_card = src->cards[src->count - n];

        if (CARD_RANK(dst_top) == CARD_RANK(src_card) + 1 &&
            CARD_COLOR(dst_top) != CARD_COLOR(src_card)) {
            return n;
        }
    }

    return 0;
}

static void
request_move_to_hold(void)
{
    if (game.cur_move.dst->count > 0) {
        show_error("Cell not empty");
        return;
    }

    game.cur_move.count = 1;
    exec_move();
}

static void
request_move_to_found(void)
{
    int expected_rank;
    card_t card;
    card_pile_st *found;

    if (game.cur_move.src == NULL) {
        start_move();
    }

    if (game.cur_move.src == NULL) {
        return;
    }

    card = CARD_SELECTED(&game);
    found = &founds[CARD_SUIT(card)];
    expected_rank = (found->count == 0) ? 0 : CARD_RANK(CARD_PILE_TOP(found)) + 1;

    if (CARD_RANK(card) != expected_rank) {
        show_error("Invalid move");
        return;
    }

    game.cur_move.dst = found;
    game.cur_move.count = 1;
    exec_move();
}

static void
request_move_to_nonempty_col(void)
{
    card_t dst_top, src_card;

    if (game.cur_move.src->type == PILE_HOLDS) {
        src_card = CARD_PILE_TOP(game.cur_move.src);
        dst_top = CARD_PILE_TOP(game.cur_move.dst);

        if (CARD_RANK(dst_top) != CARD_RANK(src_card) + 1 ||
            CARD_COLOR(dst_top) == CARD_COLOR(src_card)) {
            show_error("Invalid move");
            return;
        }

        game.cur_move.count = 1;
    } else if (game.cur_move.src->type == PILE_COLUMNS) {
        game.cur_move.count = get_move_count(game.cur_move.src, game.cur_move.dst);

        if (game.cur_move.count == 0) {
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

    if (game.cur_move.src->type == PILE_HOLDS) {
        game.cur_move.count = 1;
        exec_move();
        return;
    }

    ASSERT(game.cur_move.src->type == PILE_COLUMNS);

    max_seq_len = get_max_valid_sequence_len(game.cur_move.src);
    max_movable = MIN(max_seq_len, get_max_movable_cards_count(game.cur_move.dst));

    if (max_movable <= 1) {
        game.cur_move.count = 1;
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

    max_seq_len = get_max_valid_sequence_len(game.cur_move.src);
    max_movable = MIN(max_seq_len, get_max_movable_cards_count(game.cur_move.dst));

    if (count == 0) {
        count = max_movable;
    }

    if (count > max_movable) {
        show_error("Too many cards");
        return;
    }

    game.cur_move.count = count;
    exec_move();
}

static void
request_move(void)
{
    game.cur_move.dst = game.cur_pile;

    if (game.cur_pile->type == PILE_HOLDS) {
        request_move_to_hold();
    } else if (game.cur_pile->count == 0) {
        request_move_to_empty_col();
    } else {
        request_move_to_nonempty_col();
    }
}

static void
handle_space(void)
{
    if (game.cur_move.src == NULL) {
        start_move();
    } else if (game.cur_move.src == game.cur_pile) {
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
    card_cursor_draw(&game, 1);

    state = STATE_DEFAULT;

    update_status();
}

static void
restart_game(void)
{
    deal_cards();
    draw_piles();
    card_cursor_draw(&game, 1);
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
        card_pile_draw(&game, game.cur_move.src);
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
init_game(void)
{
    int i;

    game.origin = &window.origin;
    game.size = &window.size;

    game.card_width = CARD_WIDTH;
    game.card_height = CARD_HEIGHT;
    game.card_step = COLUMN_CARDS_STEP;

    game.cur_move.src = NULL;
    game.cur_move.dst = NULL;
    game.cur_move.count = 0;
    game.cur_pile = &columns[0];

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
        init_game();

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
