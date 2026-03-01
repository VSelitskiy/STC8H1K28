/*
  basic_v12.ino — STC8H1K28 usage example for CrowPanel Advance (revision v1.1 / v1.2)

  Supported boards:
    CrowPanel Advance ESP32-S3 4.3"  (revision v1.1)
    CrowPanel Advance ESP32-S3 5.0"  (revision v1.1)
    CrowPanel Advance ESP32-S3 7.0"  (revision v1.2)

  Wiring:
    SDA — GPIO 15
    SCL — GPIO 16
    I2C address — 0x30

  IMPORTANT:
    Do NOT call Wire.begin() manually. LovyanGFX initializes Wire(15, 16)
    internally during gfx.init(). Call panel.begin() only after gfx.init().

  Brightness control — 6 fixed steps:

    Number | Constant             | Command | Description
    -------|----------------------|---------|-------------
      0    | Brightness::OFF      |  0x05   | Backlight off
      1    | Brightness::MIN      |  0x06   | Minimum
      1    | Brightness::LEVEL_1  |  0x06   | = MIN
      2    | Brightness::LEVEL_2  |  0x07   |
      3    | Brightness::LEVEL_3  |  0x08   |
      4    | Brightness::LEVEL_4  |  0x09   |
      5    | Brightness::MAX      |  0x10   | Maximum

  Buzzer control:
    buzzerBeep(ms)  — beep for ms milliseconds (non-blocking)
    buzzerStop()    — stop immediately
    Calling buzzerBeep() while buzzer is playing restarts the timer.

  Speaker amplifier control (NS4168):
    speakerOn()   — enable amplifier power
    speakerOff()  — disable amplifier power
    Audio stream is managed separately via ESP32-S3 I2S interface.
*/

#include <Wire.h>
#include "STC8H1K28_v12.h"

// LovyanGFX initializes Wire(15, 16) — we use the same Wire instance
STC8H1K28_v12 panel(0x30, Wire);

// Stub for gfx — replace with your actual LGFX instance
// LGFX gfx;

void setup() {
    Serial.begin(115200);

    // gfx.init() must be called first — it initializes Wire(15, 16)
    // gfx.init();

    // Initialize panel after gfx.init()
    if (!panel.begin()) {
        Serial.println("ERROR: STC8H1K28 init failed");
        while (1);
    }

    panel.setBrightness(Brightness::MAX);  // maximum brightness
    panel.buzzerBeep(300);                 // startup beep
    panel.speakerOn();                     // enable speaker amplifier

    Serial.println("Setup done");
}

void loop() {
    // Step through brightness levels every second
    static uint32_t last_ms = 0;
    static uint8_t  step    = 0;

    if (millis() - last_ms >= 1000) {
        last_ms = millis();
        panel.setBrightness(step);
        Serial.printf("Brightness: %d\n", step);
        step = (step + 1) % 6;

        panel.buzzerBeep(50);  // short beep on each step
    }
}
