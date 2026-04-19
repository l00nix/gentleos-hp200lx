/*
 * Copyright (c) 2025-2026 luke8086
 * Distributed under the terms of GPL-2 License
 *
 * File: grid.c - Grid utilities
 */

#include <gui.h>

global void
gui_grid_rect(grid_st *grid, rect_st *out)
{
    out->x = grid->x;
    out->y = grid->y;
    out->width = GRID_WIDTH_SPACED(grid->cell_width, grid->cols);
    out->height = GRID_HEIGHT_SPACED(grid->cell_height, grid->rows);
}

global void
gui_grid_cell_rect(grid_st *grid, int col, int row, rect_st *out)
{
    out->x = grid->x + col * grid->cell_width + col;
    out->y = grid->y + row * grid->cell_height + row;
    out->width = grid->cell_width;
    out->height = grid->cell_height;
}

global void
gui_grid_draw_background(grid_st *grid, window_st *window, uint8_t color)
{
    rect_st grid_rect;
    gui_grid_rect(grid, &grid_rect);
    gui_surface_draw_rect(&window->origin, &grid_rect, color);
    gui_surface_mark_dirty(&window->origin, &grid_rect);
}
