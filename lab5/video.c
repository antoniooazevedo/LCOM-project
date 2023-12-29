#include "video.h"
#include "math.h"


uint16_t (get_hres)() {
    return hres;
}

int16_t (get_vres)() {
    return vres;
}

int (vg_start)(uint16_t mode) {
    memset(&mode_info, 0, sizeof(mode_info));
    if (vbe_get_mode_info(mode, &mode_info) != 0) return 1;

    hres = mode_info.XResolution;
    vres = mode_info.YResolution;

    bytes_per_pixel = (mode_info.BitsPerPixel + 7) / 8;
    size_t frame_size = hres * vres * bytes_per_pixel;

    struct minix_mem_range mr;
    unsigned int vram_base = mode_info.PhysBasePtr; 
    unsigned int vram_size = frame_size;  
                        
    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != 0) return 1;

    video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

    if (video_mem == MAP_FAILED ) return 1;

    buffer = malloc(frame_size);
    
    reg86_t reg86;
    memset(&reg86, 0, sizeof(reg86_t));

    reg86.intno = 0x10; 
    reg86.ah = 0x4F;
    reg86.al = 0x02;

    reg86.bx = BIT(14) | mode;

    if (sys_int86(&reg86) != 0) return 1;

    return 0;
}

void (normalize_color)(uint32_t *color) {
    if (mode_info.BitsPerPixel != 32)
        *color &= BIT(mode_info.BitsPerPixel) - 1;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
    if (x < 0 || y < 0) return 1;
    if (x >= hres || y >= vres) return 1;

    size_t idx = (hres * y + x) * bytes_per_pixel;
    
    if (memcpy(&buffer[idx], &color, bytes_per_pixel) == NULL) return 1;
    return 0;
}

int (vg_copy_buffer)() {
    if (memcpy(video_mem, buffer, hres * vres * bytes_per_pixel) == NULL) return 1;
    memset(buffer, 0, hres * vres * bytes_per_pixel);
    return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (int i = 0; i < len; i++) {
        if (vg_draw_pixel(x + i, y, color) != 0) return 1;
    }

    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (int i = 0; i < height; i++) {
        if (vg_draw_hline(x, y + i, width, color) != 0) return 1;
    }

    return 0;
}

int (vg_display_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    xpm_image_t img;

    uint8_t *colors = xpm_load(xpm, XPM_INDEXED, &img); 

    for (int i = 0; i < img.height; i++) {
        for (int j = 0; j < img.width; j++) {
            if (vg_draw_pixel(x + j, y + i, *colors) != 0) return 1;
            colors++;
        }
    }

    return 0;
}

int (vg_erase_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    xpm_image_t img;

    uint8_t *colors = xpm_load(xpm, XPM_INDEXED, &img); 

    for (int i = 0; i < img.height; i++) {
        for (int j = 0; j < img.width; j++) {
            if (vg_draw_pixel(x + j, y + i, 0) != 0) return 1;
            colors++;
        }
    } 
    
    return 0;
}


/* Auxiliary color functions */

uint8_t (get_memory_model)() {
    return mode_info.MemoryModel;
}

uint32_t (direct_mode)(uint32_t R, uint32_t G, uint32_t B) {
  return (R << mode_info.RedFieldPosition) | (G << mode_info.GreenFieldPosition) | (B << mode_info.BlueFieldPosition);
}

uint32_t (indexed_mode)(uint16_t col, uint16_t row, uint8_t step, uint32_t first, uint8_t n) {
  return (first + (row * n + col) * step) % (1 << mode_info.BitsPerPixel);
}

uint32_t (Red)(unsigned j, uint8_t step, uint32_t first) {
  return (R(first) + j * step) % (1 << mode_info.RedMaskSize);
}

uint32_t (Green)(unsigned i, uint8_t step, uint32_t first) {
  return (G(first) + i * step) % (1 << mode_info.GreenMaskSize);
}

uint32_t (Blue)(unsigned j, unsigned i, uint8_t step, uint32_t first) {
  return (B(first) + (i + j) * step) % (1 << mode_info.BlueMaskSize);
}

uint32_t (R)(uint32_t first){
  return ((1 << mode_info.RedMaskSize) - 1) & (first >> mode_info.RedFieldPosition);
}

uint32_t (G)(uint32_t first){
  return ((1 << mode_info.GreenMaskSize) - 1) & (first >> mode_info.GreenFieldPosition);
}

uint32_t (B)(uint32_t first){
  return ((1 << mode_info.BlueMaskSize) - 1) & (first >> mode_info.BlueFieldPosition);
}
