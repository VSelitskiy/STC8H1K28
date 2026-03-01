/*
  crowpanel_advance_7_v12.ino
  Full integration example for CrowPanel Advance ESP32-S3 7.0" (revision v1.2)
  with LovyanGFX and LVGL.

  Wiring:
    SDA — GPIO 15
    SCL — GPIO 16
    I2C address — 0x30

  Wire initialization order:
    gfx.init() initializes Wire(15, 16) for GT911 touchscreen internally.
    panel.begin() must be called AFTER gfx.init().

  Dependencies (platformio.ini):
    lib_deps =
        lvgl/lvgl @ ^8.3.11
        lovyan03/LovyanGFX @ ^1.1.12
        https://github.com/VSelitskiy/STC8H1K28.git

  build_flags:
    -I include
    -D LV_CONF_INCLUDE_SIMPLE
    -DCONFIG_SPIRAM_SPEED_120M=1
    -fno-strict-aliasing
*/

#include "LovyanGFX_Driver.h"  // your LGFX config
#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>
#include "STC8H1K28_v12.h"

// —————————————————————————————————————————
// Objects
// —————————————————————————————————————————

// Wire is initialized by LovyanGFX (GT911 touchscreen)
// panel reuses the same Wire instance after gfx.init()
STC8H1K28_v12 panel(0x30, Wire);
LGFX gfx;

// —————————————————————————————————————————
// LVGL buffers (allocated in PSRAM)
// —————————————————————————————————————————
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;
static lv_color_t *buf1;

uint16_t touch_x, touch_y;

// —————————————————————————————————————————
// LVGL callbacks
// —————————————————————————————————————————
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    if (gfx.getStartCount() > 0) {
        gfx.endWrite();
    }
    gfx.pushImageDMA(
        area->x1, area->y1,
        area->x2 - area->x1 + 1,
        area->y2 - area->y1 + 1,
        (lgfx::rgb565_t *)&color_p->full
    );
    lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    data->state = LV_INDEV_STATE_REL;
    if (gfx.getTouch(&touch_x, &touch_y)) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch_x;
        data->point.y = touch_y;
    }
}

// —————————————————————————————————————————
// Setup
// —————————————————————————————————————————
void setup() {
    Serial.begin(115200);

#if defined(CONFIG_SPIRAM_SPEED_120M)
    Serial.println("PSRAM speed: 120MHz");
#else
    Serial.println("PSRAM speed: 80MHz");
#endif
    Serial.printf("PSRAM size: %d MB\n", ESP.getPsramSize() / 1024 / 1024);

    // — Allocate LVGL buffers in PSRAM ————
    size_t buffer_size = sizeof(lv_color_t) * LV_HOR_RES_MAX * LV_VER_RES_MAX;
    buf  = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
    buf1 = (lv_color_t *)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
    if (!buf || !buf1) {
        Serial.println("ERROR: LVGL buffer allocation failed");
        while (1);
    }

    // — Display ———————————————————————————
    // gfx.init() also initializes Wire(15, 16) for GT911
    gfx.init();
    gfx.initDMA();
    gfx.startWrite();
    gfx.fillScreen(TFT_BLACK);

    // — LVGL ——————————————————————————————
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, buf1, LV_HOR_RES_MAX * LV_VER_RES_MAX);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res  = LV_HOR_RES_MAX;
    disp_drv.ver_res  = LV_VER_RES_MAX;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    gfx.fillScreen(TFT_BLACK);

    // — STC8H1K28 —————————————————————————
    // Wire is ready after gfx.init() — initialize panel now
    if (!panel.begin()) {
        Serial.println("ERROR: STC8H1K28 init failed");
        while (1);
    }
    panel.setBrightness(Brightness::MAX);
    panel.buzzerBeep(300);   // startup beep
    panel.speakerOn();       // enable speaker amplifier

    Serial.println("Setup done");
}

// —————————————————————————————————————————
// Loop
// —————————————————————————————————————————
void loop() {
    lv_timer_handler();
    delay(1);
}
