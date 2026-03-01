# STC8H1K28

Arduino library for the STC8H1K28 I2C expander found on **CrowPanel Advance** series displays.  
Provides non-blocking display brightness control and buzzer management for hardware revisions **v1.1**, **v1.2**, and **v1.3**.

### Supported boards

| Board                      | Size   | Revision | Driver file        |
|----------------------------|--------|----------|--------------------|
| CrowPanel Advance ESP32-S3 | 4.3"   | v1.1     | `STC8H1K28_v12.h`  |
| CrowPanel Advance ESP32-S3 | 5.0"   | v1.1     | `STC8H1K28_v12.h`  |
| CrowPanel Advance ESP32-S3 | 7.0"   | v1.2     | `STC8H1K28_v12.h`  |
| CrowPanel Advance ESP32-S3 | 7.0"   | v1.3     | `STC8H1K28_v13.h`  |

> **Note:** Revisions v1.1 (4.3" and 5.0") and v1.2 (7.0") use identical protocol — both are supported by `STC8H1K28_v12.h`.

---

## Library structure

```
STC8H1K28/
  src/
    STC8H1K28_v12.h / .cpp   — driver for revisions v1.1 and v1.2
    STC8H1K28_v13.h / .cpp   — driver for revision v1.3
  examples/
    basic_v12/basic_v12.ino  — usage example for v1.1 and v1.2
    basic_v13/basic_v13.ino  — usage example for v1.3
  library.json               — PlatformIO metadata
  library.properties         — Arduino IDE metadata
  README.md
```

---

## Installation (PlatformIO)

Add to your `platformio.ini`:

```ini
lib_deps =
    https://github.com/VSelitskiy/STC8H1K28.git
```

---

## Hardware differences between revisions

| Parameter   | v1.1 / v1.2               | v1.3               |
|-------------|---------------------------|--------------------|
| Brightness  | 6 fixed steps             | Smooth, 0–245      |
| Buzzer ON   | `0x15`                    | `246`              |
| Buzzer OFF  | `0x16`                    | `247`              |

---

## Wiring

| Board                      | Size | SDA     | SCL     | I2C Address |
|----------------------------|------|---------|---------|-------------|
| CrowPanel Advance ESP32-S3 | 4.3" | GPIO 15 | GPIO 16 | 0x30        |
| CrowPanel Advance ESP32-S3 | 5.0" | GPIO 15 | GPIO 16 | 0x30        |
| CrowPanel Advance ESP32-S3 | 7.0" | GPIO 15 | GPIO 16 | 0x30        |

---

## Quick start

```cpp
#include <Wire.h>
#include "STC8H1K28_v12.h"  // or STC8H1K28_v13.h for revision v1.3

STC8H1K28_v12 panel;        // or STC8H1K28_v13

void setup() {
    Wire.begin(15, 16);      // SDA, SCL
    panel.begin();
}
```

---

## API

### Common to both versions

```cpp
bool begin();                          // Initialize (creates FreeRTOS timer)
void end();                            // Deinitialize and free resources
bool buzzerBeep(uint32_t duration_ms); // Beep for duration_ms milliseconds (non-blocking)
bool buzzerStop();                     // Stop buzzer immediately
bool speakerOn();                      // Enable speaker amplifier power (NS4168)
bool speakerOff();                     // Disable speaker amplifier power
```

> **Note:** `buzzerBeep()` is non-blocking. Calling it while the buzzer is already playing
> immediately stops the current beep and starts a new one with the new duration.

> **Note:** `speakerOn()` / `speakerOff()` only control power to the NS4168 amplifier.
> The audio stream itself is managed separately via the ESP32-S3 I2S interface.

---

### v1.1 / v1.2 — stepped brightness

```cpp
bool setBrightness(Brightness level);  // set by named constant
bool setBrightness(uint8_t level);     // set by number 0–5
```

| Number | Constant             | Command | Description     |
|--------|----------------------|---------|-----------------|
| 0      | `Brightness::OFF`    | `0x05`  | Backlight off   |
| 1      | `Brightness::MIN`    | `0x06`  | Minimum         |
| 1      | `Brightness::LEVEL_1`| `0x06`  | = MIN           |
| 2      | `Brightness::LEVEL_2`| `0x07`  |                 |
| 3      | `Brightness::LEVEL_3`| `0x08`  |                 |
| 4      | `Brightness::LEVEL_4`| `0x09`  |                 |
| 5      | `Brightness::MAX`    | `0x10`  | Maximum         |

```cpp
panel.setBrightness(3);                // by number
panel.setBrightness(Brightness::MAX);  // by constant
```

---

### v1.3 — smooth brightness

```cpp
bool setBrightness(uint8_t brightness); // 0 = off, 245 = max
```

Values above 245 are automatically clamped to 245  
(246 and 247 are reserved for buzzer commands).

```cpp
panel.setBrightness(0);    // backlight off
panel.setBrightness(128);  // 50%
panel.setBrightness(245);  // maximum
```

---

## Dependencies

- ESP32 Arduino core
- FreeRTOS (included with ESP32 Arduino core)
