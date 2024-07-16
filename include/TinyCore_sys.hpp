#pragma once

#include <stdlib.h>
#include <TinyCore_config.hpp>

#if defined(STM32G0xx)
#include <stm32g0xx_hal.h>
#elif defined(STM32F4xx)
#include <stm32f4xx_hal.h>
#endif

#define delay(ms) HAL_Delay(ms)
#define delay_us(us) HAL_DelayUs(us)
#define millis() HAL_GetTick()

#ifdef USE_UART_PRINTF
#include <stdio.h>
#endif

#if defined USE_UART_PRINTF && defined USE_ERROR_DEBUG
#define DEBUG_LOG(str, ...) printf(str, __VA_ARGS__)
#define DEBUG_LOG_STR(str) printf(str)
#else
#define DEBUG_LOG(str, ...) void(nullptr)
#define DEBUG_LOG_STR(str) void(nullptr)
#endif

#ifdef USE_ERROR_DEBUG
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

extern bool UART_Print(const char *str);

__WEAK void _Error_Handler(const char *msg, int val) {
#ifdef USE_UART_PRINTF
    printf("Error: %s (%i)\n", msg, val);
#else
    UART_Print("Error: ");
    UART_Print(msg);
    UART_Print("(");
    char buff[34];
    UART_Print(itoa(val, buff, 10));
    UART_Print(")\n");
#endif
    while (1) {
    }
}

#else
#define Error_Handler()                                                                                                \
    while (1) {                                                                                                        \
    }
#endif

#ifdef USE_FULL_ASSERT
__WEAK void assert_failed(uint8_t *file, uint32_t line) { _Error_Handler((const char *)file, line); }
#endif

void SystemClock_Config();
void Systen_Init();
void HAL_MspInit(void);
void HAL_DelayUs(uint32_t us);
