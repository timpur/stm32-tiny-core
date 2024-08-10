#pragma once

#include <stm32g0xx_hal_tim.h>
#include <TinyCore_config.hpp>
#include <TinyCore_sys.hpp>
#include <TinyCore_gpio.hpp>

#ifdef HAL_TIM_MODULE_ENABLED

#ifndef PWM_RESOLUTION
#define PWM_RESOLUTION 10
#endif
#ifndef PWM_FREQUENCY
#define PWM_FREQUENCY 1000
#endif

#define PWM_MAX_VALUE ((1 << PWM_RESOLUTION) - 1)

#define PWM_TIM_PRESCALER ((CPU_FREQ / PWM_FREQUENCY * 10 / (1 << PWM_RESOLUTION) + 5) / 10)
#define PWM_TIM_RESOLUTION (1UL << PWM_RESOLUTION)

constexpr uint32_t PWM_PIN_TO_CHANNEL(GPIO_TypeDef *port, uint16_t pin);

TIM_HandleTypeDef TimerHandler = {
    .Instance = TIM1,
    .Init =
        {
            .Prescaler = PWM_TIM_PRESCALER - 1,
            .CounterMode = TIM_COUNTERMODE_UP,
            .Period = PWM_TIM_RESOLUTION - 1,
            .ClockDivision = TIM_CLOCKDIVISION_DIV1,
            .RepetitionCounter = 0,
            .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE,
        },
};

void PWM_Init() {
    if (HAL_TIM_PWM_Init(&TimerHandler) != HAL_OK) {
        Error_Handler();
    }

    TIM_ClockConfigTypeDef sClockSourceConfig = {.ClockSource = TIM_CLOCKSOURCE_INTERNAL};
    if (HAL_TIM_ConfigClockSource(&TimerHandler, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }

    TIM_MasterConfigTypeDef sMasterConfig = {
        .MasterOutputTrigger = TIM_TRGO_RESET,
        .MasterOutputTrigger2 = TIM_TRGO2_RESET,
        .MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE,
    };
    if (HAL_TIMEx_MasterConfigSynchronization(&TimerHandler, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }

    TIM_OC_InitTypeDef sConfigOC = {
        .OCMode = TIM_OCMODE_PWM1,
        .Pulse = 0,
        .OCPolarity = TIM_OCPOLARITY_HIGH,
        .OCNPolarity = TIM_OCNPOLARITY_HIGH,
        .OCFastMode = TIM_OCFAST_DISABLE,
        .OCIdleState = TIM_OCIDLESTATE_RESET,
        .OCNIdleState = TIM_OCNIDLESTATE_RESET,
    };
    if (HAL_TIM_PWM_ConfigChannel(&TimerHandler, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&TimerHandler, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&TimerHandler, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&TimerHandler, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) {
        Error_Handler();
    }
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {
        .OffStateRunMode = TIM_OSSR_DISABLE,
        .OffStateIDLEMode = TIM_OSSI_DISABLE,
        .LockLevel = TIM_LOCKLEVEL_OFF,
        .DeadTime = 0,
        .BreakState = TIM_BREAK_DISABLE,
        .BreakPolarity = TIM_BREAKPOLARITY_HIGH,
        .BreakFilter = 0,
        .BreakAFMode = TIM_BREAK_AFMODE_INPUT,
        .Break2State = TIM_BREAK2_DISABLE,
        .Break2Polarity = TIM_BREAK2POLARITY_HIGH,
        .Break2Filter = 0,
        .Break2AFMode = TIM_BREAK_AFMODE_INPUT,
        .AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE,
    };
    if (HAL_TIMEx_ConfigBreakDeadTime(&TimerHandler, &sBreakDeadTimeConfig) != HAL_OK) {
        Error_Handler();
    }
}

void GPIO_Mode_PWM(GPIO_TypeDef *port, uint16_t pin) {
    uint32_t channel = PWM_PIN_TO_CHANNEL(port, pin);
    if (channel == TIM_CHANNEL_ALL)
        Error_Handler();

    if (port == GPIOA && (pin == GPIO_PIN_8 || pin == GPIO_PIN_10 || pin == GPIO_PIN_11)) {
        GPIO_Mode_AlternateFn(port, pin, GPIO_AF2_TIM1);
    } else if (port == GPIOB && pin == GPIO_PIN_3) {
        GPIO_Mode_AlternateFn(port, pin, GPIO_AF1_TIM1);
    }

    if (HAL_TIM_PWM_Start(&TimerHandler, channel) != HAL_OK) {
        Error_Handler();
    }
}

void PWM_Set_Value(GPIO_TypeDef *port, uint16_t pin, uint16_t value) {
    uint32_t channel = PWM_PIN_TO_CHANNEL(port, pin);
    if (channel == TIM_CHANNEL_ALL)
        return;
    if (value >= PWM_MAX_VALUE)
        value = PWM_TIM_RESOLUTION;
    __HAL_TIM_SET_COMPARE(&TimerHandler, channel, value);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *timerHandler) {
    if (timerHandler->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
    }
}

constexpr uint32_t PWM_PIN_TO_CHANNEL(GPIO_TypeDef *port, uint16_t pin) {
    if (port == GPIOA) {
        switch (pin) {
        case GPIO_PIN_8:
            return TIM_CHANNEL_1;
        case GPIO_PIN_10:
            return TIM_CHANNEL_3;
        case GPIO_PIN_11:
            return TIM_CHANNEL_4;
        };
    } else if (port == GPIOB) {
        switch (pin) {
        case GPIO_PIN_3:
            return TIM_CHANNEL_2;
        };
    }
    return TIM_CHANNEL_ALL;
}

#endif
