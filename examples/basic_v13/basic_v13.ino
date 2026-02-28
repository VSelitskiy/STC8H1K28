/*
  basic_v13.ino — пример использования STC8H1K28 для CrowPanel Advance 7 (ревизия 1.3)

  Подключение:
    SDA — GPIO 8
    SCL — GPIO 9
    I2C адрес — 0x38

  Управление яркостью:
    Плавная регулировка значением 0–245 (0 = выкл, 245 = макс).
    Значения 246 и 247 зарезервированы под команды баззера —
    библиотека автоматически обрезает brightness до 245 если передать больше.

  Управление баззером:
    buzzerBeep(ms)  — включить на ms миллисекунд (non-blocking)
    buzzerStop()    — принудительно остановить
    Повторный вызов buzzerBeep() пока баззер играет — перезапускает таймер.
*/

#include <Wire.h>
#include "STC8H1K28_v13.h"

STC8H1K28_v13 panel;  // адрес 0x38 по умолчанию

void setup() {
    Serial.begin(115200);
    Wire.begin(8, 9);  // SDA, SCL для CrowPanel Advance 7

    if (!panel.begin()) {
        Serial.println("Ошибка: не удалось инициализировать STC8H1K28");
        while (1);
    }

    panel.setBrightness(128);  // 50% яркость

    // Бип 500 мс при старте
    panel.buzzerBeep(500);

    Serial.println("Инициализация завершена");
}

void loop() {
    // Плавное нарастание и спад яркости (non-blocking логика через millis)
    static uint32_t last_ms  = 0;
    static uint8_t  bright   = 0;
    static int8_t   dir      = 5;   // шаг изменения яркости

    if (millis() - last_ms >= 20) {
        last_ms = millis();

        panel.setBrightness(bright);

        if (bright == 0) {
            dir = 5;
            panel.buzzerBeep(100);  // бип в нижней точке
        } else if (bright >= 245) {
            dir = -5;
            panel.buzzerBeep(100);  // бип в верхней точке
        }

        bright += dir;
    }
}
