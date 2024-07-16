#pragma once

#include <TinyCore.hpp>

class TinyInterval {
  private:
    uint16_t _interval = 0;
    uint32_t _last_time = 0;

  public:
    TinyInterval() {}

    void setup(uint16_t interval_ms) {
        _interval = interval_ms;
        _last_time = millis();
    }

    bool enabled() { return _interval > 0; }

    void reset() { _last_time = millis(); }

    uint32_t sinceReset() { return millis() - _last_time; }

    bool loop(bool reset = true) {
        if (!enabled())
            return false;
        uint32_t now = millis();
        if (now - _last_time > _interval) {
            if (reset)
                _last_time = now;
            return true;
        }
        return false;
    }
};
