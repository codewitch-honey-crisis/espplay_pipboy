#include <Arduino.h>

#include "lcd_config.h"
#define LCD_IMPLEMENTATION
#include <gfx.hpp>
#include <uix.hpp>

#include "lcd_init.h"
#include "ui.hpp"
using namespace gfx;
using namespace uix;
using lcd_pixel_t = rgb_pixel<LCD_BIT_DEPTH>;
using color_t = color<lcd_pixel_t>;
using uix_color_t = color<rgba_pixel<32>>;
// lcd data
static const size_t lcd_transfer_buffer_size =
    bitmap<lcd_pixel_t>::sizeof_buffer({LCD_WIDTH, LCD_HEIGHT / 10});
// for sending data to the display
static uint8_t *lcd_transfer_buffer = nullptr;
#ifdef LCD_DMA
static uint8_t *lcd_transfer_buffer2 = nullptr;
#endif

uix::display disp;

void lcd_buffers_init() {
    // initialize the transfer buffers
    lcd_transfer_buffer = (uint8_t *)malloc(lcd_transfer_buffer_size);
    if (lcd_transfer_buffer == nullptr) {
        puts("Out of memory initializing primary transfer buffer");
        while (1) vTaskDelay(5);
    }
    memset(lcd_transfer_buffer, 0, lcd_transfer_buffer_size);
#ifdef LCD_DMA
    // initialize the transfer buffers
    lcd_transfer_buffer2 = (uint8_t *)malloc(lcd_transfer_buffer_size);
    if (lcd_transfer_buffer2 == nullptr) {
        puts("Out of memory initializing transfer buffer 2");
        while (1) vTaskDelay(5);
    }
    memset(lcd_transfer_buffer2, 0, lcd_transfer_buffer_size);
#endif
}

static bool lcd_flush_ready(esp_lcd_panel_io_handle_t panel_io,
                            esp_lcd_panel_io_event_data_t *edata,
                            void *user_ctx) {
    disp.flush_complete();
    return true;
}

void uix_on_flush(const rect16 &bounds, const void *bitmap, void *state) {
    lcd_panel_draw_bitmap(bounds.x1, bounds.y1, bounds.x2, bounds.y2,
                          (void *)bitmap);
#ifndef LCD_DMA
    disp.flush_complete();
#endif
}

void setup() {
    Serial.begin(115200);
    lcd_buffers_init();
    disp.buffer_size(lcd_transfer_buffer_size);
    disp.buffer1(lcd_transfer_buffer);
#ifdef LCD_DMA
    disp.buffer2(lcd_transfer_buffer2);
#endif
    disp.on_flush_callback(uix_on_flush);
    lcd_panel_init(lcd_transfer_buffer_size, lcd_flush_ready);
    ui_title_screen(disp);
    ui_pip_screen(disp);
}

void loop() {
    ui_pip_update();
    disp.update();
}
