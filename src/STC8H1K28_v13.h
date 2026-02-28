#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

class STC8H1K28_v13 {
public:
    STC8H1K28_v13(uint8_t i2c_addr = 0x38, TwoWire &wire = Wire);

    bool begin();
    void end();

    // Установить яркость дисплея: 0–245 (0 = выкл, 245 = макс), non-blocking
    bool setBrightness(uint8_t brightness);

    // Включить баззер на duration_ms миллисекунд, non-blocking.
    // Если баззер уже играет — немедленно останавливает и запускает заново.
    bool buzzerBeep(uint32_t duration_ms);

    // Принудительно остановить баззер
    bool buzzerStop();

private:
    uint8_t       _addr;
    TwoWire      &_wire;
    TimerHandle_t _buzzer_timer;

    bool _i2cWrite(uint8_t value);
    static void _buzzerTimerCallback(TimerHandle_t xTimer);
};
