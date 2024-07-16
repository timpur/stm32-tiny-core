#pragma once

#include <TinyCore_config.hpp>
#include <TinyCore_sys.hpp>
#include <TinyCore_gpio.hpp>
#include <TinyCore_pwm.hpp>
#include <TinyCore_adc.hpp>
#include <TinyCore_uart.hpp>
#include <TinyCore_i2c.hpp>
#include <TinyCore_onewire.hpp>

extern void setup();
extern void loop();

int main() {
    Systen_Init();

#if defined USE_UART_PRINTF || defined USE_ERROR_DEBUG
    UART_Init();
#endif

    // Run program
    setup();
    while (1) {
        loop();
    }
}
