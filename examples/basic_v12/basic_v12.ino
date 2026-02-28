/*
  basic_v12.ino — пример использования STC8H1K28 для CrowPanel Advance 7 (ревизия 1.2)

  Подключение:
    SDA — GPIO 8
    SCL — GPIO 9
    I2C адрес — 0x38

  Управление яркостью:
    6 фиксированных ступеней. Можно задавать числом (0–5) или именованной константой.

    Число │ Константа          │ Команда
    ──────┼────────────────────┼────────
      0   │ Brightness::OFF    │ 0x05
      1   │ Brightness::MIN    │ 0x06
      1   │ Brightness::LEVEL_1│ 0x06
      2   │ Brightness::LEVEL_2│ 0x08
      3   │ Brightness::LEVEL_3│ 0x0A
      4   │ Brightness::LEVEL_4│ 0x0C
      5   │ Brightness::MAX    │ 0x10

  Управление баззером:
    buzzerBeep(ms)  — включить на ms миллисекунд (non-blocking)
    buzzerStop()    — принудительно остановить
    Повторный вызов buzzerBeep() пока баззер играет — перезапускает таймер.
*/

#include <Wire.h>
#include "STC8H1K28_v12.h"

STC8H1K28_v12 panel;  // адрес 0x38 по умолчанию

void setup() {
    Serial.begin(115200);
    Wire.begin(8, 9);  // SDA, SCL для CrowPanel Advance 7

    if (!panel.begin()) {
        Serial.println("Ошибка: не удалось инициализировать STC8H1K28");
        while (1);
    }

    // Установка яркости через число
    panel.setBrightness(3);               // LEVEL_3

    // Установка яркости через константу
    panel.setBrightness(Brightness::MAX); // Максимум

    // Бип 500 мс при старте
    panel.buzzerBeep(500);

    Serial.println("Инициализация завершена");
}

void loop() {
    // Плавное изменение яркости по ступеням (non-blocking логика через millis)
    static uint32_t last_ms = 0;
    static uint8_t  step    = 0;

    if (millis() - last_ms >= 1000) {
        last_ms = millis();
        panel.setBrightness(step);
        Serial.printf("Яркость: %d\n", step);
        step = (step + 1) % 6;

        // Короткий бип на каждом шаге
        panel.buzzerBeep(50);
    }
}
