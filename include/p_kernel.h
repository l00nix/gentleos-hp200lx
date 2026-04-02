/* kernel/debug.c */
extern int krn_debug_text_mode_enabled;
extern void krn_debug_printf(const char *fmt, ...);
extern void krn_debug_beep_adv(unsigned hz, unsigned msecs, unsigned count);
extern void krn_debug_beep(void);
/* kernel/event.c */
extern int krn_event_ipush(event_st *event);
extern int krn_event_push(event_st *event);
extern int krn_event_pop(event_st *event);
extern uint16_t krn_event_count(void);
/* kernel/heap.c */
extern void krn_heap_init(void);
/* kernel/keyboard.c */
extern void krn_keyboard_init(void);
/* kernel/main.c */
extern void krn_main(void);
/* kernel/rtc.c */
extern int krn_rtc_are_times_equal(time_st *t1, time_st *t2);
extern void krn_rtc_get_time(time_st *t);
/* kernel/speaker.c */
extern void krn_speaker_stop(void);
extern void krn_speaker_play(uint16_t hz);
/* kernel/system.c */
extern uint32_t krn_system_get_total_mem(void);
extern uint32_t krn_system_get_used_mem(void);
extern uint32_t krn_system_get_avail_mem(void);
/* kernel/timer.c */
extern volatile uint8_t krn_timer_is_cpu_idle;
extern uint32_t krn_timer_get_msecs(void);
extern uint8_t krn_timer_get_cpu_usage(void);
extern void krn_timer_init(void);
