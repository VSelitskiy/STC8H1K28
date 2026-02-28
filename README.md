# STC8H1K28

Arduino-библиотека для управления I2C-экспандером STC8H1K28 на плате **CrowPanel Advance 7**, **CrowPanel Advance 5**, **CrowPanel Advance 4.3**.  
Поддерживает управление яркостью дисплея и баззером для аппаратных ревизий **v1.1**, **v1.2** и **v1.3**.

---

## Структура библиотеки

```
STC8H1K28/
  src/
    STC8H1K28_v12.h / .cpp   — драйвер для ревизий 1.1 и 1.2
    STC8H1K28_v13.h / .cpp   — драйвер для ревизии 1.3
  examples/
    basic_v12/basic_v12.ino  — пример для ревизий 1.1 и 1.2
    basic_v13/basic_v13.ino  — пример для ревизии 1.3
  library.json               — метаданные PlatformIO
  library.properties         — метаданные Arduino IDE
  README.md
```

---

## Подключение в PlatformIO

В `platformio.ini` проекта:

```ini
lib_deps =
    symlink:///абсолютный/путь/до/STC8H1K28
```

Симлинк позволяет редактировать библиотеку в одном месте — все проекты получают изменения автоматически.

---

## Аппаратные отличия ревизий

| Параметр              | v1.1 / v1.2                 | v1.3                  |
|-----------------------|-----------------------------|-----------------------|
| Яркость               | 6 фиксированных ступеней    | Плавная, 0–245        |
| Баззер ON             | `0x15`                      | `246`                 |
| Баззер OFF            | `0x16`                      | `247`                 |

---

## Подключение к плате

| Сигнал | GPIO (CrowPanel Advance 7) |
|--------|---------------------------|
| SDA    | GPIO 8                    |
| SCL    | GPIO 9                    |
| Адрес  | 0x38                      |

---

## Быстрый старт

```cpp
#include <Wire.h>
#include "STC8H1K28_v12.h"  // или STC8H1K28_v13.h

STC8H1K28_v12 panel;        // или STC8H1K28_v13

void setup() {
    Wire.begin(8, 9);
    panel.begin();
}
```

---

## API

### Общее для обеих версий

```cpp
bool begin();                        // Инициализация (создаёт FreeRTOS таймер)
void end();                          // Деинициализация
bool buzzerBeep(uint32_t duration_ms); // Бип duration_ms миллисекунд (non-blocking)
bool buzzerStop();                   // Принудительно остановить баззер
```

> `buzzerBeep()` — non-blocking. Повторный вызов пока баззер играет немедленно
> останавливает текущий отсчёт и запускает новый.

---

### Версия v1.1/1.2 — яркость ступенями

```cpp
// Через именованную константу
bool setBrightness(Brightness level);

// Через число 0–5
bool setBrightness(uint8_t level);
```

| Число | Константа            | Описание       |
|-------|----------------------|----------------|
| 0     | `Brightness::OFF`    | Подсветка выкл |
| 1     | `Brightness::MIN`    | Минимум        |
| 1     | `Brightness::LEVEL_1`| = MIN          |
| 2     | `Brightness::LEVEL_2`|                |
| 3     | `Brightness::LEVEL_3`|                |
| 4     | `Brightness::LEVEL_4`|                |
| 5     | `Brightness::MAX`    | Максимум       |

```cpp
panel.setBrightness(3);               // по числу
panel.setBrightness(Brightness::MAX); // по константе
```

---

### Версия v1.3 — яркость плавная

```cpp
bool setBrightness(uint8_t brightness); // 0 = выкл, 245 = макс
```

Значения выше 245 автоматически обрезаются до 245  
(246 и 247 зарезервированы под команды баззера).

```cpp
panel.setBrightness(0);    // выключить подсветку
panel.setBrightness(128);  // 50%
panel.setBrightness(245);  // максимум
```

---

## Зависимости

- ESP32 Arduino core
- FreeRTOS (входит в состав ESP32 Arduino core)
