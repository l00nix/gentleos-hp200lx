/* gui/button.c */
extern void gui_button_draw(widget_st *widget);
/* gui/grid.c */
extern void gui_grid_rect(grid_st *grid, rect_st *out);
extern void gui_grid_cell_rect(grid_st *grid, int col, int row, rect_st *out);
extern void gui_grid_draw_background(grid_st *grid, window_st *window, uint8_t color);
/* gui/main.c */
extern rect_st gui_app_rect;
extern app_st *gui_current_app;
extern void gui_run_app(app_st *app);
extern void gui_main(void);
/* gui/rect.c */
extern const rect_st GUI_RECT_ZERO;
extern const rect_st GUI_RECT_SCREEN;
extern const point_st GUI_POINT_ZERO;
extern void gui_rect_copy(rect_st *dst, const rect_st *src);
extern void gui_point_copy(point_st *dst, const point_st *src);
extern void gui_size_copy(size_st *dst, const size_st *src);
extern int gui_rect_is_empty(const rect_st *r);
extern void gui_rect_init(rect_st *out, int x, int y, int width, int height);
extern void gui_rect_translate(rect_st *r, const point_st *v);
extern void gui_rect_translate_back(rect_st *r, const point_st *v);
extern int gui_rect_contains_point(const rect_st *r, const point_st *p);
extern void gui_rect_center(rect_st *r, const rect_st *container);
extern void gui_rect_limit(rect_st *r, const rect_st *container);
extern void gui_rect_shrink(rect_st *r, int amount);
extern void gui_rect_enclose(rect_st *a, const rect_st *b);
extern void gui_rect_clip(rect_st *r, const rect_st *clipper);
extern const char * gui_rect_format(const rect_st *r);
/* gui/status.c */
extern void gui_status_set_tl(const char *fmt, ...);
extern void gui_status_set_tr(const char *fmt, ...);
extern void gui_status_set(const char *fmt, ...);
extern void gui_status_set_br(const char *fmt, ...);
extern void gui_status_init(void);
/* gui/surface.c */
extern void gui_surface_init(void);
extern void gui_surface_mark_dirty(const point_st *origin, const rect_st *rect);
extern void gui_surface_flush(void);
extern void gui_surface_draw_h_seg(const point_st *origin, int x, int y, int w, uint8_t color);
extern void gui_surface_draw_v_seg(const point_st *origin, int x, int y, int h, uint8_t color);
extern void gui_surface_draw_border(const point_st *origin, const rect_st *r, uint8_t color);
extern void gui_surface_draw_rect(const point_st *origin, const rect_st *rect, uint8_t color);
extern void gui_surface_scroll_up(const point_st *origin, const rect_st *rect, int dy);
extern void gui_surface_draw_char(const point_st *origin, uint16_t x, uint16_t y, font_st *font, uint8_t ch, uint8_t fg, uint8_t bg);
extern void gui_surface_draw_str(const point_st *origin, uint16_t x, uint16_t y, font_st *font, const char *s, uint8_t fg, uint8_t bg);
extern void gui_surface_draw_str_centered(const point_st *origin, const rect_st *rect, font_st *font, const char *s, uint8_t fg, uint8_t bg);
extern void gui_surface_draw_bitmap(const point_st *origin, const size_st *bounds, int dst_x, int dst_y, bitmap_st *bitmap, uint8_t fill);
extern void gui_surface_draw_bitmap_centered(const point_st *origin, const size_st *bounds, const rect_st *rect, bitmap_st *bitmap, uint8_t fill);
/* gui/widget.c */
extern void gui_widget_draw(widget_st *widget);
/* gui/window.c */
extern void gui_window_rect(window_st *window, rect_st *out);
extern void gui_window_area(window_st *window, rect_st *out);
extern void gui_window_init(window_st *window, int width, int height);
extern void gui_window_draw(window_st *window);
extern int gui_window_add_widget(window_st *window, widget_st *widget);
extern void gui_window_on_close(window_st *window);
extern void gui_window_on_key_down(window_st *window, const event_st *event);
extern void gui_window_on_key_up(window_st *window, const event_st *event);
extern void gui_window_on_uart_rx(window_st *window, const event_st *event);
extern void gui_window_on_tick(window_st *window);
