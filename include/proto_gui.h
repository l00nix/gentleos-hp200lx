/* gui/button.c */
extern void gui_button_draw(widget_st *widget);
/* gui/fb.c */
extern surface_st *gui_fb_vram_surface;
extern void gui_fb_draw_start(void);
extern void gui_fb_draw_end(void);
extern void gui_fb_mark_dirty(rect_st rect);
extern void gui_fb_draw_rect(rect_st rect, uint8_t color);
extern void gui_fb_draw_pattern(rect_st rect, bitmap_st *pattern, uint8_t c1, uint8_t c2);
extern void gui_fb_draw_surface(int dst_x, int dst_y, surface_st *src_sf, rect_st src_rect);
extern void gui_fb_flush(void);
extern void gui_fb_init(void);
/* gui/grid.c */
extern rect_st gui_grid_rect(grid_st *grid);
extern rect_st gui_grid_cell_rect(grid_st *grid, int col, int row);
extern void gui_grid_draw_background(grid_st *grid, window_st *window, uint8_t color);
/* gui/main.c */
extern void gui_main(void);
/* gui/planar.c */
extern void gui_planar_flush(rect_st rect);
extern void gui_planar_draw_rect(rect_st rect, uint8_t color);
extern void gui_planar_draw_pattern(rect_st dst_rect, bitmap_st *pattern, uint8_t c1, uint8_t c2);
extern void gui_planar_draw_surface(int dst_x, int dst_y, surface_st *src, rect_st src_rect);
/* gui/rect.c */
extern int gui_rect_is_empty(rect_st r);
extern rect_st gui_rect_make(int x, int y, int width, int height);
extern rect_st gui_rect_translate(rect_st r, point_st v);
extern rect_st gui_rect_translate_back(rect_st r, point_st v);
extern int gui_rect_contains_point(rect_st r, point_st p);
extern rect_st gui_rect_center(rect_st r, rect_st container);
extern rect_st gui_rect_limit(rect_st r, rect_st container);
extern rect_st gui_rect_shrink(rect_st r, int amount);
extern rect_st gui_rect_enclose(rect_st a, rect_st b);
extern rect_st gui_rect_clip(rect_st r, rect_st clipper);
extern const char *gui_rect_format(rect_st r);
/* gui/status.c */
extern void gui_status_set(const char *fmt, ...);
extern void gui_status_set_alert(const char *fmt, ...);
extern void gui_status_init(void);
/* gui/surface.c */
extern void gui_surface_draw_h_seg(surface_st *surface, int x, int y, int w, uint8_t color);
extern void gui_surface_draw_v_seg(surface_st *surface, int x, int y, int h, uint8_t color);
extern void gui_surface_draw_border(surface_st *surface, rect_st r, uint8_t color);
extern void gui_surface_draw_rect(surface_st *surface, rect_st r, uint8_t color);
extern void gui_surface_draw_char(surface_st *surface, uint16_t x, uint16_t y, font_st *font, uint8_t ch, uint8_t fg, uint8_t bg);
extern void gui_surface_draw_str(surface_st *surface, uint16_t x, uint16_t y, font_st *font, const char *s, uint8_t fg, uint8_t bg);
extern void gui_surface_draw_str_centered(surface_st *surface, rect_st rect, font_st *font, const char *s, uint8_t fg, uint8_t bg);
extern void gui_surface_draw_bitmap_1bpp(surface_st *surface, rect_st src_rect, int dst_x, int dst_y, bitmap_st *bitmap, uint8_t fill);
extern void gui_surface_draw_bitmap(surface_st *surface, int dst_x, int dst_y, bitmap_st *bitmap, uint8_t fill);
extern void gui_surface_draw_bitmap_centered(surface_st *surface, rect_st rect, bitmap_st *bitmap, uint8_t fill);
extern void gui_surface_draw_pattern(surface_st *surface, rect_st reg, bitmap_st *b, uint8_t col1, uint8_t col2);
/* gui/timeout.c */
extern void gui_timeout_remove(uint64_t id);
extern int gui_timeout_add(uint32_t msecs, timeout_callback_fn callback, timeout_payload payload);
extern void gui_timeout_on_tick(event_st event);
/* gui/vga.c */
extern void gui_vga_init(void);
/* gui/widget.c */
extern void gui_widget_draw(widget_st *widget);
/* gui/window.c */
extern rect_st gui_window_area(window_st *window);
extern void gui_window_init_frame(window_st *window);
extern void gui_window_draw(window_st *window);
extern int gui_window_add_widget(window_st *window, widget_st *widget);
extern void gui_window_on_close(window_st *window);
extern void gui_window_on_key_down(window_st *window, event_st event);
/* gui/wm.c */
extern rect_st gui_wm_container;
extern bitmap_st *gui_wm_bg_pattern;
extern int gui_wm_add_window(struct window *w);
extern void gui_wm_render_window_surface(window_st *window, rect_st desktop_reg);
extern void gui_wm_render_desktop_region(rect_st rect, window_st *bottom_window);
extern void gui_wm_render_window_region(window_st *window, rect_st window_reg);
extern window_st *gui_wm_find_window(uint16_t x, uint16_t y);
extern window_st *gui_wm_top_window(void);
extern void gui_wm_set_status_window(window_st *w);
extern void gui_wm_init(void);
