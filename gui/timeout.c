/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: timeouts.c - Scheduled callbacks
 */

#include <gui.h>

#define TIMEOUTS_MAX_COUNT 32

typedef struct {
    uint32_t msecs;
    timeout_callback_fn callback;
    void *payload;

    uint32_t id;
    uint32_t initialized;
    uint32_t added_at;
    uint32_t expires_at;
} timeout_st;

static timeout_st timeouts[TIMEOUTS_MAX_COUNT];
static unsigned timeout_count = 0;
static uint32_t timeout_id = 1;

void
gui_timeout_remove(uint32_t id)
{
    size_t i;

    for (i = 0; i < TIMEOUTS_MAX_COUNT; ++i) {
        if (timeouts[i].id == id) {
            break;
        }
    }

    if (i == TIMEOUTS_MAX_COUNT) {
        return;
    }

    --timeout_count;

    for (; i < timeout_count; ++i) {
        memcpy(&timeouts[i], &timeouts[i + 1], sizeof(timeouts[i]));
    }
}

int
gui_timeout_add(uint32_t msecs, timeout_callback_fn callback, timeout_payload payload)
{
    uint32_t id;

    if (timeout_count >= TIMEOUTS_MAX_COUNT) {
        return 0;
    }

    id = timeout_id++;

    if (timeout_id == 0) {
        ++timeout_id;
    }

    timeouts[timeout_count].id = id;
    timeouts[timeout_count].initialized = 0;
    timeouts[timeout_count].msecs = msecs;
    timeouts[timeout_count].callback = callback;
    timeouts[timeout_count].payload = payload;
    timeout_count++;

    return id;
}

void
gui_timeout_on_tick(const event_st *event)
{
    timeout_st *tm;
    uint32_t i;

    for (i = 0; i < timeout_count; i++) {
        tm = &timeouts[i];

        if (tm->initialized == 0) {
            tm->initialized = 1;
            tm->added_at = event->payload.timer.timer_msecs;
            tm->expires_at = event->payload.timer.timer_msecs + tm->msecs;
            continue;
        }

        /* Check timeout that was scheduled before overflow of the timer */
        if (tm->expires_at >= tm->added_at &&
            (event->payload.timer.timer_msecs >= tm->expires_at || event->payload.timer.timer_msecs < tm->added_at)) {

            tm->callback(tm->payload);
            gui_timeout_remove(tm->id);
            break;
        }

        /* Check timeout that was scheduled after overflow of the timer */
        if (tm->expires_at < tm->added_at &&
            (event->payload.timer.timer_msecs >= tm->expires_at && event->payload.timer.timer_msecs < tm->added_at)) {

            tm->callback(tm->payload);
            gui_timeout_remove(tm->id);
            break;
        }
    }
}

void
gui_timeout_init(void)
{
    memset(timeouts, 0, sizeof(timeouts));
    timeout_count = 0;
    timeout_id = 1;
}
