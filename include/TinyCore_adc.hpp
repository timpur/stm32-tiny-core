#pragma once

#include <TinyCore_config.hpp>
#include <TinyCore_sys.hpp>
#include <TinyCore_gpio.hpp>

#if defined(STM32G0xx)
#include <stm32g0xx_hal.h>
#include <stm32g0xx_hal_gpio.h>
#elif defined(STM32F4xx)
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>
#endif

#ifdef HAL_ADC_MODULE_ENABLED

extern ADC_HandleTypeDef AdcHandle;

#define ADC_TO_MV(val) __HAL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, val, ADC_RESOLUTION_12B)

void ADC_Init();

uint16_t ADC_Read(uint32_t channel);

uint16_t ADC_Read(GPIO_TypeDef *port, uint16_t pin);

void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle);

constexpr uint32_t ADC_PIN_TO_CHANNEL(GPIO_TypeDef *port, uint16_t pin);

#endif
