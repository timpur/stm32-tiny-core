// #include <TinyCore.hpp>
#include <TinyCore_sys.hpp>
#if defined USE_UART_PRINTF || defined USE_ERROR_DEBUG
#include <TinyCore_uart.hpp>
#endif

extern void init();
extern void setup();
extern void loop();

int main() {
    init();
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

__weak void init() {}
