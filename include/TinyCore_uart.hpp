#pragma once

#include <string.h>
#include "stm32g0xx_hal_uart.h"
#include <TinyCore_config.hpp>
#include <TinyCore_sys.hpp>
#include <TinyCore_gpio.hpp>

// Uart 2 is used by default for the system

#ifdef HAL_UART_MODULE_ENABLED

UART_HandleTypeDef UartHandle = {
    .Instance = USART2,
    .Init =
        {
            .BaudRate = 115200,
            .WordLength = UART_WORDLENGTH_8B,
            .StopBits = UART_STOPBITS_1,
            .Parity = UART_PARITY_NONE,
            .Mode = UART_MODE_TX_RX,
            .HwFlowCtl = UART_HWCONTROL_NONE,
            .OverSampling = UART_OVERSAMPLING_16,
        },
};

void UART_Init(UART_HandleTypeDef *uartHandle = &UartHandle) {
    if (HAL_UART_Init(uartHandle) != HAL_OK) {
        Error_Handler();
    }
}

bool UART_Print(uint8_t *msg, size_t len, uint32_t timeout = 100) {
    return HAL_UART_Transmit(&UartHandle, msg, len, timeout) != HAL_OK;
}
bool UART_Print(const char *str, uint32_t timeout) { return UART_Print((uint8_t *)str, strlen(str), timeout); }
bool UART_Print(const char *str) { return UART_Print(str, 100); }

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle) {
#ifdef USE_UART_1
    if (uartHandle->Instance == USART1) {
        RCC_PeriphCLKInitTypeDef periphClkInit = {
            .PeriphClockSelection = RCC_PERIPHCLK_USART1,
            .Usart1ClockSelection = RCC_USART1CLKSOURCE_SYSCLK,
        };

        if (HAL_RCCEx_PeriphCLKConfig(&periphClkInit) != HAL_OK) {
            Error_Handler();
        }

        __HAL_RCC_USART1_CLK_ENABLE();

        GPIO_Mode_AlternateFn(GPIOA, GPIO_PIN_9 | GPIO_PIN_10, GPIO_AF1_USART1, GPIO_PULLUP);
    } else
#endif
        if (uartHandle->Instance == USART2) {
        __HAL_RCC_USART2_CLK_ENABLE();

        GPIO_Mode_AlternateFn(GPIOA, GPIO_PIN_2 | GPIO_PIN_3, GPIO_AF1_USART2, GPIO_PULLUP);
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart) {
#ifdef USE_UART_1
    if (huart->Instance == USART1) {
        __HAL_RCC_USART1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);
    } else
#endif
        if (huart->Instance == USART2) {
        __HAL_RCC_USART2_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);
    }
}

#ifdef USE_UART_PRINTF
extern "C" {
#include <errno.h>
#include <stdio.h>
#include <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO
int _write(int file, char *data, int len) {
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }
    HAL_UART_Transmit(&UartHandle, (uint8_t *)data, (uint16_t)len, 0xFFFF);
    return len;
}
}
#endif

#endif
