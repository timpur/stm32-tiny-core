#pragma once

#include <TinyCore_config.hpp>

#if defined(STM32G0xx)
#include <stm32g0xx_hal.h>
#include <stm32g0xx_hal_gpio.h>
#elif defined(STM32F4xx)
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>
#endif

#ifdef HAL_GPIO_MODULE_ENABLED

void GPIO_Init(GPIO_TypeDef *port, uint16_t pin, uint32_t mode, uint8_t pullMode = GPIO_NOPULL, bool openDrain = false,
               uint8_t speed = GPIO_SPEED_FREQ_LOW, uint8_t altFn = 0);

void GPIO_Mode_Input(GPIO_TypeDef *port, uint16_t pin, uint8_t pullMode = GPIO_NOPULL);

void GPIO_Mode_Output(GPIO_TypeDef *port, uint16_t pin, bool openDrain = false, uint8_t pullMode = GPIO_NOPULL,
                      uint8_t speed = GPIO_SPEED_FREQ_LOW);

void GPIO_Mode_Analog(GPIO_TypeDef *port, uint16_t pin, uint8_t pullMode = GPIO_NOPULL);

void GPIO_Mode_AlternateFn(GPIO_TypeDef *port, uint16_t pin, uint8_t altFn, uint8_t pullMode = GPIO_NOPULL,
                           bool openDrain = false, uint8_t speed = GPIO_SPEED_FREQ_LOW);

bool GPIO_Read(GPIO_TypeDef *port, uint16_t pin);

void GPIO_Write(GPIO_TypeDef *port, uint16_t pin, bool state);

void GPIO_Toggle(GPIO_TypeDef *port, uint16_t pin);

#endif
