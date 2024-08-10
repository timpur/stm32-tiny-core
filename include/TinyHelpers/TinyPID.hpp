#pragma once

// Inspiration (Credit) from https://github.com/mike-matera/FastPID/tree/main

#include <TinyCore.hpp>

#define TINY_PID_BYPASS_VALUE 80 // out of 100

#define TINY_PID_INTEG_MAX (INT32_MAX)
#define TINY_PID_INTEG_MIN (INT32_MIN)
#define TINY_PID_DERIV_MAX (INT16_MAX)
#define TINY_PID_DERIV_MIN (INT16_MIN)

#ifndef TINY_PID_PARAM_SHIFT
#define TINY_PID_PARAM_SHIFT 8
#endif
#define TINY_PID_PARAM_BITS 16
#define TINY_PID_PARAM_MAX (((0x1ULL << TINY_PID_PARAM_BITS) - 1) >> TINY_PID_PARAM_SHIFT)
#define TINY_PID_PARAM_MULT (((0x1ULL << TINY_PID_PARAM_BITS)) >> (TINY_PID_PARAM_BITS - TINY_PID_PARAM_SHIFT))

constexpr uint32_t TINY_PID_F_TO_P(float in) {
    if (in > TINY_PID_PARAM_MAX || in < 0) {
        return 0;
    }
    uint32_t param = in * TINY_PID_PARAM_MULT;
    if (in != 0 && param == 0) {
        return 0;
    }
    return param;
}

class TinyPid {
  private:
    int32_t _p = 0, _i = 0, _d = 0;
    uint32_t _outmax = 0;
    int16_t _target = 0, _last_target = 0;
    int32_t _last_err = 0;
    int64_t _i_err = 0;
    uint16_t _last_result = 0;

  public:
    TinyPid() {}

    void setup(float p, float i, float d, uint16_t interval, uint16_t outmax) {
        _p = TINY_PID_F_TO_P(p);
        _i = TINY_PID_F_TO_P(i * (float(interval) / 1000.0f));
        _d = TINY_PID_F_TO_P(d / (float(interval) / 1000.0f));
        _outmax = uint64_t(outmax) * TINY_PID_PARAM_MULT;
    }

    void setTarget(int16_t target) { _target = target; }

    uint16_t step(int16_t input) {
        // DEBUG_LOG("PID: Input=%i\n", input);

        if (int32_t(input) < (int32_t(_target) * TINY_PID_BYPASS_VALUE / 100)) {
            // DEBUG_LOG_STR("PID: Bypass\n");
            _last_target = 0;
            _last_err = 0;
            _i_err = 0;
            return _outmax;
        }

        // int16 + int16 = int17
        int32_t err = int32_t(_target) - int32_t(input);

        // uint16 * int16 = int32
        int32_t P = int32_t(_p) * int32_t(err);

        // int17 * int16 = int33
        _i_err += int64_t(err) * int64_t(_i);

        // Limit sum to 32-bit signed value so that it saturates, never overflows.
        if (_i_err > TINY_PID_INTEG_MAX)
            _i_err = TINY_PID_INTEG_MAX;
        else if (_i_err < TINY_PID_INTEG_MIN)
            _i_err = TINY_PID_INTEG_MIN;

        // int32
        int32_t I = _i_err;

        // (int17 - int16) - (int16 - int16) = int19
        int32_t deriv = (err - _last_err) - int32_t(_target - _last_target);
        _last_target = _target;
        _last_err = err;

        // Limit the derivative to 16-bit signed value.
        if (deriv > TINY_PID_DERIV_MAX)
            deriv = TINY_PID_DERIV_MAX;
        else if (deriv < TINY_PID_DERIV_MIN)
            deriv = TINY_PID_DERIV_MIN;

        // int16 * int16 = int32
        int32_t D = int32_t(_d) * int32_t(deriv);

        // int32 (P) + int32 (I) + int32 (D) = int34
        int64_t out = int64_t(P) + int64_t(I) + int64_t(D);

        // Make the output saturate
        if (out > _outmax)
            out = _outmax;
        else if (out < 0)
            out = 0;

        // Remove the integer scaling factor.
        uint16_t result = out >> TINY_PID_PARAM_SHIFT;
        // Fair rounding.
        if (out & (0x1ULL << (TINY_PID_PARAM_SHIFT - 1))) {
            result++;
        }
        // Rolling avg
        result += _last_result;
        result /= 2;
        _last_result = result;

        return result;
    }
};
