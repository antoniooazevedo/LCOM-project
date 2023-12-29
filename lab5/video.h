#ifndef VIDEO_H
#define VIDEO_H

#include <lcom/lcf.h>
#include "vbe.h"

static vbe_mode_info_t mode_info;
static uint8_t *video_mem;
static uint8_t *buffer;

static uint16_t hres, vres;
static uint8_t bytes_per_pixel;

int (vg_start)(uint16_t mode);

uint16_t (get_hres)();

int (vg_copy_buffer)();
int16_t (get_vres)();

void (normalize_color)(uint32_t *color);

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

int (vg_display_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y);

int (vg_erase_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y);

uint8_t (get_memory_model)();

uint32_t (direct_mode)(uint32_t R, uint32_t G, uint32_t B);

uint32_t (indexed_mode)(uint16_t col, uint16_t row, uint8_t step, uint32_t first, uint8_t n);

uint32_t (Red)(unsigned j, uint8_t step, uint32_t first);

uint32_t (Green)(unsigned i, uint8_t step, uint32_t first);

uint32_t (Blue)(unsigned i, unsigned j, uint8_t step, uint32_t first);

uint32_t (R)(uint32_t color);

uint32_t (G)(uint32_t color);

uint32_t (B)(uint32_t color);

#endif
