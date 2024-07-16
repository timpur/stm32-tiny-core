#pragma once

#include <TinyCore.hpp>

#ifndef NTC_THERMISTOR_FACTOR_M
#define NTC_THERMISTOR_FACTOR_M -29
#endif

#ifndef NTC_THERMISTOR_FACTOR_B
#define NTC_THERMISTOR_FACTOR_B 86957
#endif

#ifndef NTC_THERMISTOR_FACTOR_CORRECTION
#define NTC_THERMISTOR_FACTOR_CORRECTION 0
#endif

#ifndef NTC_THERMISTOR_FACTOR_SAMPLES
#define NTC_THERMISTOR_FACTOR_SAMPLES 10
#endif

class NtcThermistor {
  private:
    uint32_t _channel = 0;

  public:
    NtcThermistor() {}

    void setup(GPIO_TypeDef *port, uint16_t pin) {
        GPIO_Mode_Analog(port, pin);
        _channel = ADC_PIN_TO_CHANNEL(port, pin);
    }

    // Raw adc value
    uint16_t rawValue() {
        uint32_t total = 0;
        for (int i = 0; i < NTC_THERMISTOR_FACTOR_SAMPLES; i++) {
            total += ADC_Read(_channel);
        }
        return total / NTC_THERMISTOR_FACTOR_SAMPLES;
    }

    // Temp value is shifted 1 dp. ie 25.0 -> 250
    int16_t value() {
        int x = rawValue();
        // DEBUG_LOG("NTC: Raw Value: %u\n", x);
        int16_t val = ((NTC_THERMISTOR_FACTOR_M * x) + NTC_THERMISTOR_FACTOR_B) / 100;
        val += NTC_THERMISTOR_FACTOR_CORRECTION;
        return val;
    }
};