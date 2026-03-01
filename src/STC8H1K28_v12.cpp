#include "STC8H1K28_v12.h"

static constexpr uint8_t CMD_BUZZER_ON  = 0x15;
static constexpr uint8_t CMD_BUZZER_OFF = 0x16;
static constexpr uint8_t CMD_SPEAKER_ON  = 0x17;
static constexpr uint8_t CMD_SPEAKER_OFF = 0x18;

constexpr uint8_t STC8H1K28_v12::BRIGHTNESS_TABLE[6];

STC8H1K28_v12::STC8H1K28_v12(uint8_t i2c_addr, TwoWire &wire)
    : _addr(i2c_addr), _wire(wire), _buzzer_timer(nullptr) {}

bool STC8H1K28_v12::begin() {
    _buzzer_timer = xTimerCreate(
        "buzzer_v12",
        pdMS_TO_TICKS(100),
        pdFALSE,
        this,
        _buzzerTimerCallback
    );
    return (_buzzer_timer != nullptr);
}

void STC8H1K28_v12::end() {
    if (_buzzer_timer) {
        xTimerDelete(_buzzer_timer, portMAX_DELAY);
        _buzzer_timer = nullptr;
    }
}

bool STC8H1K28_v12::setBrightness(Brightness level) {
    return _i2cWrite(static_cast<uint8_t>(level));
}

bool STC8H1K28_v12::setBrightness(uint8_t level) {
    if (level > 5) level = 5;
    return _i2cWrite(BRIGHTNESS_TABLE[level]);
}

bool STC8H1K28_v12::buzzerBeep(uint32_t duration_ms) {
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

bool STC8H1K28_v12::buzzerStop() {
    if (!_buzzer_timer) return false;
    xTimerStop(_buzzer_timer, 0);
    return _i2cWrite(CMD_BUZZER_OFF);
}

bool STC8H1K28_v12::speakerOn() {
    return _i2cWrite(CMD_SPEAKER_ON);
}

bool STC8H1K28_v12::speakerOff() {
    return _i2cWrite(CMD_SPEAKER_OFF);
}

bool STC8H1K28_v12::_i2cWrite(uint8_t value) {
    _wire.beginTransmission(_addr);
    _wire.write(value);
    return (_wire.endTransmission() == 0);
}

void STC8H1K28_v12::_buzzerTimerCallback(TimerHandle_t xTimer) {
    STC8H1K28_v12 *self = static_cast<STC8H1K28_v12 *>(pvTimerGetTimerID(xTimer));
    if (self) {
        self->_i2cWrite(CMD_BUZZER_OFF);
    }
}
