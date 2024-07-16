#pragma once

#include <TinyCore.hpp>
#include <TinyHelpers/TinyInterval.hpp>

#define TINY_LED_MAX_VALUE PWM_MAX_VALUE

class TinyLed {
  private:
    GPIO_TypeDef *_port;
    uint16_t _pin;
    TinyInterval _step_interval;
    int16_t _step_value = 0;
    uint16_t _val = 0;

    uint16_t _iabs(int16_t v) {
        if (v < 0)
            return v * -1;
        return v;
    }

    uint32_t _ipow(uint32_t x, int n) {
        uint32_t number = 1;
        for (int i = 0; i < n; ++i)
            number *= x;
        return number;
    }

    uint16_t _gamma_correct(uint16_t x) {
        if (x >= TINY_LED_MAX_VALUE)
            return TINY_LED_MAX_VALUE;
        return (_ipow(x, 2) / TINY_LED_MAX_VALUE);
    }

  public:
    TinyLed() {}
    void setup(GPIO_TypeDef *port, uint16_t pin) {
        _port = port;
        _pin = pin;
        GPIO_Mode_PWM(_port, _pin);
    }

    void loop() {
        if (_step_interval.loop()) {
            int32_t val = int32_t(_val) + _step_value;
            if (val <= 0 || val >= TINY_LED_MAX_VALUE) {
                val = val <= 0 ? 0 : TINY_LED_MAX_VALUE;
                _step_interval.setup(0); // Stop updating;
            }
            setVal(val);
        }
    }

    uint16_t get() { return _val; }

    bool isOn() { return _val > 0; }

    void setVal(uint16_t val) {
        if (val >= TINY_LED_MAX_VALUE)
            val = TINY_LED_MAX_VALUE;
        _val = val;
        uint16_t corrected_val = _gamma_correct(_val);
        PWM_Set_Value(_port, _pin, corrected_val);
    }

    void set(bool val) { setVal(val ? TINY_LED_MAX_VALUE : 0); }

    bool fadeInProgress() { return _step_interval.enabled(); }

    void fadeTo(uint16_t fade_ms, uint16_t val) {
        int16_t steps_to_take = val - _val;
        if (fade_ms < steps_to_take) {
            _step_interval.setup(1);
            _step_value = _iabs(steps_to_take) / fade_ms;
        } else {
            _step_value = 1;
            _step_interval.setup(fade_ms / _iabs(steps_to_take));
        }
        if (steps_to_take < 0)
            _step_value *= -1;
    }

    void fade(uint16_t fade_ms, bool val) { return fadeTo(fade_ms, val ? TINY_LED_MAX_VALUE : 0); }
};
