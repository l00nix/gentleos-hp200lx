/*
 * Copyright (c) 2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: bjack.c - Blackjack game
 */

#include <gui.h>

enum {
    SPACING = 10,

    CARD_WIDTH = 30,
    CARD_HEIGHT = 45,
    CARD_SPACING = 4,
    CARDS_X = SPACING,
    CARDS_MAX = 6,
    CARDS_WIDTH = CARDS_MAX * CARD_WIDTH + (CARDS_MAX - 1) * CARD_SPACING,
    CARDS_DEALER_Y = TITLE_BAR_HEIGHT + SPACING,
    DIVIDER_Y = CARDS_DEALER_Y + CARD_HEIGHT + SPACING,
    CARDS_PLAYER_Y = DIVIDER_Y + 1 + SPACING,

    WINDOW_WIDTH = CARDS_X + CARDS_WIDTH + SPACING,

    BUTTON_WIDTH = 60,
    BUTTON_HEIGHT = 28,
    BUTTON_SPACING = SPACING * 2,
    BUTTONS_Y = CARDS_PLAYER_Y + CARD_HEIGHT + SPACING,
    BUTTONS_TOTAL_W = 2 * BUTTON_WIDTH + BUTTON_SPACING,
    BUTTON_HIT_X = (WINDOW_WIDTH - BUTTONS_TOTAL_W) / 2,
    BUTTON_STAND_X = BUTTON_HIT_X + BUTTON_WIDTH + BUTTON_SPACING,
    BUTTON_DEAL_X = (WINDOW_WIDTH - BUTTON_WIDTH) / 2,

    WINDOW_HEIGHT = BUTTONS_Y + BUTTON_HEIGHT + SPACING,

    HAND_SIZE_MAX = 11, /* 4*A + 4*2 + 3*3 */
    DECK_SIZE = 52,
};

enum {
    STATE_PLAYING = 0,
    STATE_OVER = 1,
};

static const char *rank_str[] = {
    "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"
};

static const char *suit_str[] = { "\x03", "\x04", "\x05", "\x06" };

static window_st window;

static widget_st hit_button;
static widget_st stand_button;
static widget_st deal_button;
static widget_st *widgets[3];

static uint8_t deck[DECK_SIZE];
static int deck_pos;

static uint8_t player_hand[HAND_SIZE_MAX];
static int player_hand_count;

static uint8_t dealer_hand[HAND_SIZE_MAX];
static int dealer_hand_count;

static const char *status_msg = "";
static int game_state;
static int wins;
static int losses;

static void
shuffle_deck(void)
{
    int i, j;
    uint8_t tmp;
    deck_pos = 0;

    for (i = 0; i < DECK_SIZE; i++) {
        deck[i] = i;
    }

    for (i = DECK_SIZE - 1; i > 0; i--) {
        j = rand() % (i + 1);
        tmp = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp;
    }
}

static uint8_t
deal_card(void)
{
    return deck[deck_pos++];
}

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
hand_score(uint8_t *hand, int count)
{
    int i;
    int score = 0;
    int aces = 0;
    int rank;

    for (i = 0; i < count; i++) {
        rank = card_rank(hand[i]);

        if (rank == 0) {
            score += 11;
            aces++;
        } else if (rank >= 10) {
            score += 10;
        } else {
            score += rank + 1;
        }
    }

    while (score > 21 && aces > 0) {
        score -= 10;
        aces--;
    }

    return score;
}

static int
is_blackjack(uint8_t *hand, int count)
{
    return count == 2 && hand_score(hand, count) == 21;
}

static void
draw_card(int x, int y, uint8_t card, int face_up)
{
    rect_st r;
    int rank, suit;
    uint8_t fg, bg;

    gui_rect_init(&r, x, y, CARD_WIDTH, CARD_HEIGHT);

    if (!face_up) {
        gui_surface_draw_rect(&window.origin, &r, COLOR_BG);
        gui_surface_draw_border(&window.origin, &r, COLOR_FG);
        gui_surface_draw_str_centered(&window.origin, &r, font_8x8,
            "?", COLOR_FG, COLOR_BG);
        return;
    }

    rank = card_rank(card);
    suit = card_suit(card);
    fg = COLOR_FG;
    bg = COLOR_BG;

    gui_surface_draw_rect(&window.origin, &r, bg);
    gui_surface_draw_border(&window.origin, &r, COLOR_FG);
    gui_surface_draw_str(&window.origin, x + 3, y + 2, font_8x8, rank_str[rank], fg, bg);
    gui_surface_draw_str_centered(&window.origin, &r, font_8x8, suit_str[suit], fg, bg);
    gui_surface_draw_str(&window.origin,
        x + CARD_WIDTH - strlen(rank_str[rank]) * 8 - 3,
        y + CARD_HEIGHT - 10,
        font_8x8, rank_str[rank], fg, bg);
}

static void
draw_hand(uint8_t *hand)
{
    int is_player = (hand == player_hand);
    int count = is_player ? player_hand_count : dealer_hand_count;
    int y = is_player ? CARDS_PLAYER_Y : CARDS_DEALER_Y;
    int all_face_up = is_player || game_state == STATE_OVER;
    int step, max_step, x, i;
    rect_st r;

    gui_rect_init(&r, CARDS_X, y, CARDS_WIDTH, CARD_HEIGHT);
    gui_surface_draw_rect(&window.origin, &r, COLOR_BG);

    step = CARD_WIDTH + CARD_SPACING;
    if (count > 1) {
        max_step = (CARDS_WIDTH - CARD_WIDTH) / (count - 1);
        step = MIN(step, max_step);
    }

    for (i = 0; i < count; i++) {
        x = CARDS_X + i * step;
        draw_card(x, y, hand[i], all_face_up || i == 0);
    }

    gui_wm_render_window_region(&window, &r);
}

static void
update_buttons(void)
{
    rect_st r;

    hit_button.hidden = game_state == STATE_OVER;
    stand_button.hidden = game_state == STATE_OVER;
    deal_button.hidden = game_state == STATE_PLAYING;

    gui_rect_init(&r, 1, BUTTONS_Y, WINDOW_WIDTH - 2, BUTTON_HEIGHT);
    gui_surface_draw_rect(&window.origin, &r, COLOR_BG);

    gui_widget_draw(&hit_button);
    gui_widget_draw(&stand_button);
    gui_widget_draw(&deal_button);

    gui_wm_render_window_region(&window, &r);
}

static void
update_status(void)
{
    int player_score = hand_score(player_hand, player_hand_count);
    int dealer_score = hand_score(dealer_hand, dealer_hand_count);

    if (game_state == STATE_PLAYING) {
        gui_status_set("Dealer: ?  You:%2d  \xb3  W:%d  L:%d",
        player_score, wins, losses);
    } else {
        gui_status_set("Dealer:%2d  You:%2d  \xb3  %s  \xb3  W:%d  L:%d",
            dealer_score, player_score, status_msg, wins, losses);
    }
}

static void
end_game(const char *msg)
{
    game_state = STATE_OVER;
    status_msg = msg;

    draw_hand(dealer_hand);
    update_buttons();
    update_status();
}

static void
restart_game(void)
{
    int player_blackjack;
    int dealer_blackjack;

    shuffle_deck();

    player_hand_count = 0;
    dealer_hand_count = 0;

    player_hand[player_hand_count++] = deal_card();
    dealer_hand[dealer_hand_count++] = deal_card();
    player_hand[player_hand_count++] = deal_card();
    dealer_hand[dealer_hand_count++] = deal_card();

    game_state = STATE_PLAYING;
    update_buttons();
    draw_hand(player_hand);
    draw_hand(dealer_hand);

    player_blackjack = is_blackjack(player_hand, player_hand_count);
    dealer_blackjack = is_blackjack(dealer_hand, dealer_hand_count);

    if (player_blackjack && dealer_blackjack) {
        end_game("Both Blackjack! Push");
    } else if (player_blackjack) {
        wins++;
        end_game("Blackjack! You win!");
    } else if (dealer_blackjack) {
        losses++;
        end_game("Dealer Blackjack!");
    } else {
        update_status();
    }
}

static void
player_stand(void)
{
    int dealer_score = hand_score(dealer_hand, dealer_hand_count);
    int player_score = hand_score(player_hand, player_hand_count);

    while (dealer_score < 17 && dealer_hand_count < HAND_SIZE_MAX) {
        dealer_hand[dealer_hand_count++] = deal_card();
        dealer_score = hand_score(dealer_hand, dealer_hand_count);
    }

    if (dealer_score > 21) {
        wins++;
        end_game("Dealer busts! You win!");
    } else if (player_score > dealer_score) {
        wins++;
        end_game("You win!");
    } else if (dealer_score > player_score) {
        losses++;
        end_game("Dealer wins");
    } else {
        end_game("Push!");
    }
}

static void
player_hit(void)
{
    int score;

    if (player_hand_count >= HAND_SIZE_MAX) {
        return;
    }

    player_hand[player_hand_count++] = deal_card();
    draw_hand(player_hand);

    score = hand_score(player_hand, player_hand_count);

    if (score > 21) {
        losses++;
        end_game("Bust! You lose");
    } else if (score == 21) {
        player_stand();
    } else {
        update_status();
    }
}

static void
on_hit_button(widget_st *widget, const event_st *event _unsd, const point_st *pos _unsd)
{
    gui_widget_draw(widget);

    if (game_state == STATE_PLAYING) {
        player_hit();
    }
}

static void
on_stand_button(widget_st *widget, const event_st *event _unsd, const point_st *pos _unsd)
{
    gui_widget_draw(widget);

    if (game_state == STATE_PLAYING) {
        player_stand();
    }
}

static void
on_deal_button(widget_st *widget, const event_st *event _unsd, const point_st *pos _unsd)
{
    gui_widget_draw(widget);

    if (game_state == STATE_OVER) {
        restart_game();
    }
}

static void
init_window(void)
{
    gui_window_init(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    window.title = "Blackjack";
    window.bg_color = COLOR_BG;
    window.widgets = widgets;
    window.widgets_capacity = sizeof(widgets) / sizeof(widgets[0]);
}

static void
init_buttons(void)
{
    hit_button.type = WIDGET_TYPE_BUTTON;
    gui_rect_init(&hit_button.rect, BUTTON_HIT_X, BUTTONS_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
    hit_button.label = "Hit";
    hit_button.on_pointer_up = on_hit_button;
    gui_window_add_widget(&window, &hit_button);

    stand_button.type = WIDGET_TYPE_BUTTON;
    gui_rect_init(&stand_button.rect, BUTTON_STAND_X, BUTTONS_Y, BUTTON_WIDTH,
        BUTTON_HEIGHT);
    stand_button.label = "Stand";
    stand_button.on_pointer_up = on_stand_button;
    gui_window_add_widget(&window, &stand_button);

    deal_button.type = WIDGET_TYPE_BUTTON;
    gui_rect_init(&deal_button.rect, BUTTON_DEAL_X, BUTTONS_Y, BUTTON_WIDTH,
        BUTTON_HEIGHT);
    deal_button.label = "Deal";
    deal_button.hidden = 1;
    deal_button.on_pointer_up = on_deal_button;
    gui_window_add_widget(&window, &deal_button);
}

static void
show_app(void)
{
    static int initialized = 0;

    if (!initialized) {
        init_window();
        init_buttons();
        initialized = 1;
    }

    gui_wm_add_window(&window);
    gui_window_draw(&window);
    gui_surface_draw_h_seg(&window.origin, 1, DIVIDER_Y, WINDOW_WIDTH - 2, COLOR_FG);

    restart_game();
}

app_st app_blackjack = {
    &bitmap_icon_blackjack,
    show_app,
};
