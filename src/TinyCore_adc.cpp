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
#ifdef USE_ADC_OVERSAMPLING
            .OversamplingMode = ENABLE,
            .Oversampling =
                {
                    .Ratio = ADC_OVERSAMPLING_RATIO_16,
                    .RightBitShift = ADC_RIGHTBITSHIFT_4,
                    .TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER,
                },
#else
            .OversamplingMode = DISABLE,
            .Oversampling = {},
#endif
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

uint16_t ADC_INT_Read_VRef() {
    uint16_t Vref_int = ADC_Read(ADC_CHANNEL_VREFINT);
    return __HAL_ADC_CALC_VREFANALOG_VOLTAGE(Vref_int, ADC_RESOLUTION_12B);
}

int16_t ADC_INT_Read_Temp() {
    int32_t temp = ADC_Read(ADC_CHANNEL_TEMPSENSOR);
    temp = (temp * VDDA_APPLI) / TEMPSENSOR_CAL_VREFANALOG;
    return (((temp - (int32_t)*TEMPSENSOR_CAL1_ADDR) * (TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP) * 10) /
            ((int32_t)*TEMPSENSOR_CAL2_ADDR - (int32_t)*TEMPSENSOR_CAL1_ADDR)) +
           (TEMPSENSOR_CAL1_TEMP * 10);
    // return __HAL_ADC_CALC_TEMPERATURE(VDDA_APPLI * 10, temp, ADC_RESOLUTION_12B);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle) {
#if defined(STM32G0xx)
    __HAL_RCC_ADC_CLK_ENABLE();
#elif defined(STM32F4xx)
    __HAL_RCC_ADC1_CLK_ENABLE();
#endif
}

#endif
