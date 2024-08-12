#pragma once

#include <string.h>
#include <TinyCore_config.hpp>
#include <TinyCore_sys.hpp>
#include <TinyCore_gpio.hpp>

#if defined(STM32G0xx)
#include "stm32g0xx_hal_uart.h"
#endif
// Uart 2 is used by default for the system

#ifdef HAL_UART_MODULE_ENABLED

extern UART_HandleTypeDef UartHandle;

void UART_Init(UART_HandleTypeDef *uartHandle = &UartHandle);

bool UART_Print(uint8_t *msg, size_t len, uint32_t timeout = 100);

bool UART_Print(const char *str, uint32_t timeout);

bool UART_Print(const char *str);

#endif
