#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

// Управление яркостью: 6 ступеней (0–5) или именованные константы
enum class Brightness : uint8_t {
    OFF     = 0x05,
    MIN     = 0x06,  // = LEVEL_1
    LEVEL_1 = 0x06,
    LEVEL_2 = 0x07,
    LEVEL_3 = 0x08,
    LEVEL_4 = 0x09,
    MAX     = 0x10
};

class STC8H1K28_v12 {
public:
    STC8H1K28_v12(uint8_t i2c_addr = 0x30, TwoWire &wire = Wire);

    bool begin();
    void end();

    // Установить яркость по имени уровня
    bool setBrightness(Brightness level);

    // Установить яркость по числу: 0 (выкл) — 5 (макс)
    bool setBrightness(uint8_t level);

    // Включить баззер на duration_ms миллисекунд, non-blocking.
    // Если баззер уже играет — немедленно останавливает и запускает заново.
    bool buzzerBeep(uint32_t duration_ms);

    // Принудительно остановить баззер
    bool buzzerStop();

    // Включить питание усилителя спикера (аудио через I2S)
    bool speakerOn();

    // Выключить питание усилителя спикера
    bool speakerOff();

private:
    static constexpr uint8_t BRIGHTNESS_TABLE[6] = {
        0x05,  // 0 — OFF
        0x06,  // 1 — MIN / LEVEL_1
        0x07,  // 2 — LEVEL_2
        0x08,  // 3 — LEVEL_3
        0x09,  // 4 — LEVEL_4
        0x10   // 5 — MAX
    };

    uint8_t       _addr;
    TwoWire      &_wire;
    TimerHandle_t _buzzer_timer;

    bool _i2cWrite(uint8_t value);
    static void _buzzerTimerCallback(TimerHandle_t xTimer);
};
