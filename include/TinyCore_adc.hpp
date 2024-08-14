#pragma once

#include <TinyCore_config.hpp>
#include <TinyCore_sys.hpp>
#include <TinyCore_gpio.hpp>

#if defined(STM32G0xx)
#include <stm32g0xx_hal_adc.h>
#include <stm32g0xx_hal_gpio.h>
#elif defined(STM32F4xx)
#include <stm32f4xx_hal_adc.h>
#include <stm32f4xx_hal_gpio.h>
#endif

#ifdef HAL_ADC_MODULE_ENABLED

#ifndef VDDA_APPLI
#define VDDA_APPLI (uint32_t(3300))
#endif

#define ADC_TO_MV(__ADC_DATA__) __HAL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, __ADC_DATA__, ADC_RESOLUTION_12B)
#define ADC_MAX_VALUE __LL_ADC_DIGITAL_SCALE(ADC_RESOLUTION_12B)

extern ADC_HandleTypeDef AdcHandle;

void ADC_Init();

uint16_t ADC_Read(uint32_t channel);

uint16_t ADC_Read(GPIO_TypeDef *port, uint16_t pin);

// Returns insternal vref in mV
uint16_t ADC_INT_Read_VRef();

// Return internal temp in deciCelsius ie 20.5 -> 205
int16_t ADC_INT_Read_Temp();

constexpr uint32_t ADC_PIN_TO_CHANNEL(GPIO_TypeDef *port, uint16_t pin) {
    if (port == GPIOA) {
        switch (pin) {
        case GPIO_PIN_0:
            return ADC_CHANNEL_0;
        case GPIO_PIN_1:
            return ADC_CHANNEL_1;
        case GPIO_PIN_2:
            return ADC_CHANNEL_2;
        case GPIO_PIN_3:
            return ADC_CHANNEL_3;
        case GPIO_PIN_4:
            return ADC_CHANNEL_4;
        case GPIO_PIN_5:
            return ADC_CHANNEL_5;
        case GPIO_PIN_6:
            return ADC_CHANNEL_6;
        case GPIO_PIN_7:
            return ADC_CHANNEL_7;
#if defined(STM32G030xx)
        case GPIO_PIN_11:
            return ADC_CHANNEL_15;
        case GPIO_PIN_12:
            return ADC_CHANNEL_16;
        case GPIO_PIN_13:
            return ADC_CHANNEL_17;
        case GPIO_PIN_14:
            return ADC_CHANNEL_18;
#endif
        };
    } else if (port == GPIOB) {
        switch (pin) {
        case GPIO_PIN_0:
            return ADC_CHANNEL_8;
        case GPIO_PIN_1:
            return ADC_CHANNEL_9;
#if defined(STM32G030xx)
        case GPIO_PIN_2:
            return ADC_CHANNEL_10;
        case GPIO_PIN_7:
            return ADC_CHANNEL_11;
        case GPIO_PIN_10:
            return ADC_CHANNEL_11;
        case GPIO_PIN_11:
            return ADC_CHANNEL_15;
        case GPIO_PIN_12:
            return ADC_CHANNEL_16;
#endif
        };
    }
#if defined(STM32F411xE)
    else if (port == GPIOC) {
        switch (pin) {
        case GPIO_PIN_0:
            return ADC_CHANNEL_10;
        case GPIO_PIN_1:
            return ADC_CHANNEL_11;
        case GPIO_PIN_2:
            return ADC_CHANNEL_12;
        case GPIO_PIN_3:
            return ADC_CHANNEL_13;
        case GPIO_PIN_4:
            return ADC_CHANNEL_14;
        case GPIO_PIN_5:
            return ADC_CHANNEL_15;
        };
    }
#endif
    return 0;
}

#endif
