#ifndef __DRV_OLED_H
#define __DRV_OLED_H
#include "main.h"
#define OLED_WIDTH  128
#define OLED_HEIGHT 64
void oled_init(I2C_HandleTypeDef *hi2c);
void oled_clear(void);
void oled_refresh(void);
void oled_set_pixel(uint8_t x, uint8_t y, uint8_t color);
void oled_show_string(uint8_t x, uint8_t y, const char *str, uint8_t size);
void oled_show_num(uint8_t x, uint8_t y, int32_t num, uint8_t len, uint8_t size);
void oled_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void oled_draw_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void oled_draw_progress_bar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t percent);
void oled_draw_lock_icon(uint8_t x, uint8_t y, uint8_t locked);
void oled_draw_wifi_icon(uint8_t x, uint8_t y, int8_t rssi);
void oled_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
#endif
