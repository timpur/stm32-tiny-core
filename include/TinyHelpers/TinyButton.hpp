#pragma once

#include <TinyCore.hpp>
#include <TinyHelpers/TinyInterval.hpp>

#define TINY_BUTTON_DEBOUNCE_MS 20
#define TINY_BUTTON_CLICK_TIMEOUT 500
#define TINY_BUTTON_INVERT true

class TinyButton;

typedef void (*TinyButtonCb)(TinyButton *sender, uint8_t clicks);

class TinyButton {
  private:
    GPIO_TypeDef *_port;
    uint16_t _pin;
    TinyButtonCb _cb = nullptr;
    TinyInterval _debounce;
    bool _last_state = 0;
    bool _debounce_state = 0;
    uint8_t _clicks = 0;

  public:
    TinyButton() {}

    void setup(GPIO_TypeDef *port, uint16_t pin, TinyButtonCb cb) {
        _port = port;
        _pin = pin;
        _cb = cb;
        _debounce.setup(TINY_BUTTON_DEBOUNCE_MS);
        GPIO_Mode_Input(_port, _pin, GPIO_PULLUP);
    }

    void loop() {
        if (_cb == nullptr)
            return;
        bool state = GPIO_Read(_port, _pin);
        if (TINY_BUTTON_INVERT)
            state = !state;
        if (state != _last_state) {
            _debounce.reset();
        }
        if (_debounce.loop(false)) {
            // if change from 0 to 1 then click
            if (!_debounce_state && state) {
                _clicks++;
                _debounce_state = state;
            } else if (_debounce_state && !state) {
                _debounce_state = state;
            }
            // if clicks and longer than timeout then handle clicks
            if (_clicks && _debounce.sinceReset() > TINY_BUTTON_CLICK_TIMEOUT) {
                _cb(this, _clicks);
                _clicks = 0;
            }
        }
        _last_state = state;
    }
};