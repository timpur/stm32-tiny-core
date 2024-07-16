#include <TinyCore_adc.hpp>

#ifdef HAL_ADC_MODULE_ENABLED

// Use ADC1 by default
ADC_HandleTypeDef AdcHandle = {
    .Instance = ADC1,
    .Init =
#if defined(STM32G030xx)
        {
            .ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2,
            .Resolution = ADC_RESOLUTION_12B,
            .DataAlign = ADC_DATAALIGN_RIGHT,
            .ScanConvMode = ADC_SCAN_DISABLE,
            .EOCSelection = ADC_EOC_SINGLE_CONV,
            .LowPowerAutoWait = DISABLE,
            .LowPowerAutoPowerOff = DISABLE,
            .ContinuousConvMode = DISABLE,
            .NbrOfConversion = 1,
            .DiscontinuousConvMode = DISABLE,
            .ExternalTrigConv = ADC_SOFTWARE_START,
            .ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE,
            .DMAContinuousRequests = DISABLE,
            .Overrun = ADC_OVR_DATA_PRESERVED,
            .SamplingTimeCommon1 = ADC_SAMPLETIME_160CYCLES_5,
            .SamplingTimeCommon2 = ADC_SAMPLETIME_160CYCLES_5,
            .OversamplingMode = DISABLE,
            .Oversampling =
                {
                    .Ratio = ADC_OVERSAMPLING_RATIO_16,
                    .RightBitShift = ADC_RIGHTBITSHIFT_4,
                    .TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER,
                },
            .TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH,
        },
#elif defined(STM32F4xx)
        {
            .ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4,
            .Resolution = ADC_RESOLUTION_12B,
            .DataAlign = ADC_DATAALIGN_RIGHT,
            .ScanConvMode = DISABLE,
            .EOCSelection = ADC_EOC_SINGLE_CONV,
            .ContinuousConvMode = DISABLE,
            .NbrOfConversion = 1,
            .DiscontinuousConvMode = DISABLE,
            .NbrOfDiscConversion = 0,
            .ExternalTrigConv = ADC_SOFTWARE_START,
            .ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE,
            .DMAContinuousRequests = DISABLE,
        },
#endif
};

void ADC_Init() {
    if (HAL_ADC_Init(&AdcHandle) != HAL_OK) {
        Error_Handler();
    }
}

uint16_t ADC_Read(uint32_t channel) {
    if (channel == 0) {
        return 0;
    }
    ADC_ChannelConfTypeDef channelConfig = {
        .Channel = channel,
#if defined(STM32G030xx)
        .Rank = ADC_REGULAR_RANK_1,
        .SamplingTime = ADC_SAMPLINGTIME_COMMON_1,
#elif defined(STM32F411xE)
        .Rank = 1,
        .SamplingTime = ADC_SAMPLETIME_144CYCLES,
#endif
    };
    if (HAL_ADC_ConfigChannel(&AdcHandle, &channelConfig) != HAL_OK) {
        return 0;
    }
#if defined(STM32G030xx)
    if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK) {
        return 0;
    }
#endif
    if (HAL_ADC_Start(&AdcHandle) != HAL_OK) {
        return 0;
    }
    if (HAL_ADC_PollForConversion(&AdcHandle, 10) != HAL_OK) {
        /* End Of Conversion flag not set on time */
        return 0;
    }
    uint16_t val = 0;
    if ((HAL_ADC_GetState(&AdcHandle) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC) {
        /*##-5- Get the converted value of regular channel  ########################*/
        val = HAL_ADC_GetValue(&AdcHandle);
    }
    if (HAL_ADC_Stop(&AdcHandle) != HAL_OK) {
        /* Stop Conversation Error */
        return 0;
    }
    return val;
}

uint16_t ADC_Read(GPIO_TypeDef *port, uint16_t pin) {
    uint32_t channel = ADC_PIN_TO_CHANNEL(port, pin);
    return ADC_Read(channel);
}

#define ADC_TO_MV(val) __HAL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, val, ADC_RESOLUTION_12B)

void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle) {
#if defined(STM32G0xx)
    __HAL_RCC_ADC_CLK_ENABLE();
#elif defined(STM32F4xx)
    __HAL_RCC_ADC1_CLK_ENABLE();
#endif
}

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
