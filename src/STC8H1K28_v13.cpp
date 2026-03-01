#include "STC8H1K28_v13.h"

static constexpr uint8_t CMD_BUZZER_ON   = 246;
static constexpr uint8_t CMD_BUZZER_OFF  = 247;
static constexpr uint8_t CMD_SPEAKER_ON  = 0x17;
static constexpr uint8_t CMD_SPEAKER_OFF = 0x18;

STC8H1K28_v13::STC8H1K28_v13(uint8_t i2c_addr, TwoWire &wire)
    : _addr(i2c_addr), _wire(wire), _buzzer_timer(nullptr) {}

bool STC8H1K28_v13::begin() {
    _buzzer_timer = xTimerCreate(
        "buzzer_v13",
        pdMS_TO_TICKS(100),
        pdFALSE,
        this,
        _buzzerTimerCallback
    );
    return (_buzzer_timer != nullptr);
}

void STC8H1K28_v13::end() {
    if (_buzzer_timer) {
        xTimerDelete(_buzzer_timer, portMAX_DELAY);
        _buzzer_timer = nullptr;
    }
}

bool STC8H1K28_v13::setBrightness(uint8_t brightness) {
    if (brightness > 245) brightness = 245;
    return _i2cWrite(brightness);
}

bool STC8H1K28_v13::buzzerBeep(uint32_t duration_ms) {
    if (!_buzzer_timer) return false;

    if (xTimerIsTimerActive(_buzzer_timer)) {
        xTimerStop(_buzzer_timer, 0);
    } else {
        if (!_i2cWrite(CMD_BUZZER_ON)) return false;
    }

    xTimerChangePeriod(_buzzer_timer, pdMS_TO_TICKS(duration_ms), 0);
    xTimerStart(_buzzer_timer, 0);
    return true;
}

bool STC8H1K28_v13::buzzerStop() {
    if (!_buzzer_timer) return false;
    xTimerStop(_buzzer_timer, 0);
    return _i2cWrite(CMD_BUZZER_OFF);
}

bool STC8H1K28_v13::speakerOn() {
    return _i2cWrite(CMD_SPEAKER_ON);
}

bool STC8H1K28_v13::speakerOff() {
    return _i2cWrite(CMD_SPEAKER_OFF);
}

bool STC8H1K28_v13::_i2cWrite(uint8_t value) {
    _wire.beginTransmission(_addr);
    _wire.write(value);
    return (_wire.endTransmission() == 0);
}

void STC8H1K28_v13::_buzzerTimerCallback(TimerHandle_t xTimer) {
    STC8H1K28_v13 *self = static_cast<STC8H1K28_v13 *>(pvTimerGetTimerID(xTimer));
    if (self) {
        self->_i2cWrite(CMD_BUZZER_OFF);
    }
}
