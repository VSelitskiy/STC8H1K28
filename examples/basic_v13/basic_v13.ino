/*
  basic_v13.ino — STC8H1K28 usage example for CrowPanel Advance (revision v1.3)

  Supported boards:
    CrowPanel Advance ESP32-S3 7.0"  (revision v1.3)

  Wiring:
    SDA — GPIO 15
    SCL — GPIO 16
    I2C address — 0x30

  IMPORTANT:
    Do NOT call Wire.begin() manually. LovyanGFX initializes Wire(15, 16)
    internally during gfx.init(). Call panel.begin() only after gfx.init().

  Brightness control — smooth, 0 to 245:
    0   = backlight off
    245 = maximum
    Values above 245 are automatically clamped to 245
    (246 and 247 are reserved for buzzer commands).

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
#include "STC8H1K28_v13.h"

// LovyanGFX initializes Wire(15, 16) — we use the same Wire instance
STC8H1K28_v13 panel(0x30, Wire);

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

    panel.setBrightness(245);  // maximum brightness
    panel.buzzerBeep(300);     // startup beep
    panel.speakerOn();         // enable speaker amplifier

    Serial.println("Setup done");
}

void loop() {
    // Smooth brightness fade up and down
    static uint32_t last_ms = 0;
    static uint8_t  bright  = 0;
    static int8_t   dir     = 5;

    if (millis() - last_ms >= 20) {
        last_ms = millis();
        panel.setBrightness(bright);

        if (bright == 0) {
            dir = 5;
            panel.buzzerBeep(100);  // beep at bottom
        } else if (bright >= 245) {
            dir = -5;
            panel.buzzerBeep(100);  // beep at top
        }

        bright += dir;
    }
}
