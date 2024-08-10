#pragma once

#include <math.h>
#include <TinyCore.hpp>

#define NTC_THERMISTOR_REF_T 25.0
#define NTC_THERMISTOR_REF_R 10000
#define NTC_THERMISTOR_R 10000
#define NTC_THERMISTOR_B_COEFFICIENT 3950
#define NTC_THERMISTOR_T_FACTOR 10
#define NTC_THERMISTOR_ERROR 10
#define K0 273.15

int32_t NTC_THERMISTOR_R_TO_T(uint32_t r);

class NtcThermistor {
  private:
    uint32_t _channel = 0;
    uint16_t _r = 0;

  public:
    NtcThermistor() {}

    void setup(GPIO_TypeDef *port, uint16_t pin, uint16_t r = NTC_THERMISTOR_R) {
        GPIO_Mode_Analog(port, pin);
        _channel = ADC_PIN_TO_CHANNEL(port, pin);
        _r = r;
    }

    // Resistance value
    uint16_t rValue() {
        uint32_t v = ADC_Read(_channel);
        uint32_t r = (v * uint32_t(_r)) / (ADC_MAX_VALUE - v);
        return r;
    }

    // Temp value is shifted n dp following NTC_THERMISTOR_T_FACTOR
    int16_t value() {
        uint16_t r = rValue();
        DEBUG_LOG("NTC: R Value: %u\n", r);
        return NTC_THERMISTOR_R_TO_T(r);
    }
};

// Convert T in degC to resistance based of b coefficient
constexpr uint32_t NTC_THERMISTOR_T_TO_R(const float t) {
    float k = t + K0;
    float k0 = NTC_THERMISTOR_REF_T + K0;
    return float(NTC_THERMISTOR_REF_R) * exp(float(NTC_THERMISTOR_B_COEFFICIENT) * (1.0 / k - 1.0 / k0));
}

// Static point of the b coefficient 0 to 100 every 5 deg
__RO const uint32_t NTC_THERMISTOR_R_MAP[] = {
    NTC_THERMISTOR_T_TO_R(-10), NTC_THERMISTOR_T_TO_R(0),   NTC_THERMISTOR_T_TO_R(5),  NTC_THERMISTOR_T_TO_R(10),
    NTC_THERMISTOR_T_TO_R(15),  NTC_THERMISTOR_T_TO_R(20),  NTC_THERMISTOR_T_TO_R(25), NTC_THERMISTOR_T_TO_R(30),
    NTC_THERMISTOR_T_TO_R(35),  NTC_THERMISTOR_T_TO_R(40),  NTC_THERMISTOR_T_TO_R(45), NTC_THERMISTOR_T_TO_R(50),
    NTC_THERMISTOR_T_TO_R(55),  NTC_THERMISTOR_T_TO_R(60),  NTC_THERMISTOR_T_TO_R(65), NTC_THERMISTOR_T_TO_R(70),
    NTC_THERMISTOR_T_TO_R(75),  NTC_THERMISTOR_T_TO_R(80),  NTC_THERMISTOR_T_TO_R(85), NTC_THERMISTOR_T_TO_R(90),
    NTC_THERMISTOR_T_TO_R(95),  NTC_THERMISTOR_T_TO_R(100),
};
#define NTC_THERMISTOR_R_MAP_INCREMENT 5
#define NTC_THERMISTOR_R_MAP_MIN -10
#define NTC_THERMISTOR_R_MAP_MAX 100
#define NTC_THERMISTOR_R_MAP_LEN (sizeof(NTC_THERMISTOR_R_MAP) / sizeof(uint32_t))

// Convert the resistance to temp using fixed point linear conversion; return t * NTC_THERMISTOR_T_FACTOR
int32_t NTC_THERMISTOR_R_TO_T(uint32_t r) {
    if (NTC_THERMISTOR_R_MAP[0] < r)
        return NTC_THERMISTOR_R_MAP_MIN * NTC_THERMISTOR_T_FACTOR;
    if (r < NTC_THERMISTOR_R_MAP[NTC_THERMISTOR_R_MAP_LEN - 1])
        return NTC_THERMISTOR_R_MAP_MAX * NTC_THERMISTOR_T_FACTOR;
    uint32_t i = 0;
    uint32_t r1, r2 = NTC_THERMISTOR_R_MAP[i];
    do {
        r1 = r2;
        r2 = NTC_THERMISTOR_R_MAP[++i];
    } while (i < NTC_THERMISTOR_R_MAP_LEN - 1 && !(r1 >= r && r >= r2));
    uint32_t r_pre_t = (r1 - r2) / 5;
    int32_t t_offset =
        (NTC_THERMISTOR_R_MAP_MIN + ((i - 1) * NTC_THERMISTOR_R_MAP_INCREMENT)) * NTC_THERMISTOR_T_FACTOR;
    return t_offset + (((r1 * NTC_THERMISTOR_T_FACTOR) - (r * NTC_THERMISTOR_T_FACTOR)) / r_pre_t);
}
