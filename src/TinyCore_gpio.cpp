
#include <TinyCore_gpio.hpp>

#ifdef HAL_GPIO_MODULE_ENABLED

void GPIO_Init(GPIO_TypeDef *port, uint16_t pin, uint32_t mode, uint8_t pullMode, bool openDrain, uint8_t speed,
               uint8_t altFn) {

    switch ((uint32_t)port) {
    case (uint32_t)GPIOA_BASE:
        __HAL_RCC_GPIOA_CLK_ENABLE();
        break;
    case (uint32_t)GPIOB_BASE:
        __HAL_RCC_GPIOB_CLK_ENABLE();
        break;
    case (uint32_t)GPIOC_BASE:
        __HAL_RCC_GPIOC_CLK_ENABLE();
        break;
    case (uint32_t)GPIOD_BASE:
        __HAL_RCC_GPIOD_CLK_ENABLE();
        break;
#ifdef GPIOE_BASE
    case (uint32_t)GPIOE_BASE:
        __HAL_RCC_GPIOE_CLK_ENABLE();
        break;
#endif
#ifdef GPIOF_BASE
    case (uint32_t)GPIOF_BASE:
        __HAL_RCC_GPIOF_CLK_ENABLE();
        break;
#endif
    }

#ifdef STM32G030xx
    if (port == GPIOA && pin == GPIO_PIN_9)
        HAL_SYSCFG_EnableRemap(SYSCFG_REMAP_PA11);
    if (port == GPIOA && pin == GPIO_PIN_10)
        HAL_SYSCFG_EnableRemap(SYSCFG_REMAP_PA12);
#endif

    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = pin,
        .Mode = mode | (mode == MODE_OUTPUT || mode == MODE_AF ? (openDrain == true ? OUTPUT_OD : OUTPUT_PP) : 0),
        .Pull = pullMode,
        .Speed = speed,
        .Alternate = altFn,
    };

    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void GPIO_Mode_Input(GPIO_TypeDef *port, uint16_t pin, uint8_t pullMode) { GPIO_Init(port, pin, MODE_INPUT, pullMode); }

void GPIO_Mode_Output(GPIO_TypeDef *port, uint16_t pin, bool openDrain, uint8_t pullMode, uint8_t speed) {
    GPIO_Init(port, pin, MODE_OUTPUT, pullMode, openDrain, speed);
}

void GPIO_Mode_Analog(GPIO_TypeDef *port, uint16_t pin, uint8_t pullMode) {
    GPIO_Init(port, pin, MODE_ANALOG, pullMode);
}

void GPIO_Mode_AlternateFn(GPIO_TypeDef *port, uint16_t pin, uint8_t altFn, uint8_t pullMode, bool openDrain,
                           uint8_t speed) {
    GPIO_Init(port, pin, MODE_AF, pullMode, openDrain, speed, altFn);
}

bool GPIO_Read(GPIO_TypeDef *port, uint16_t pin) { return HAL_GPIO_ReadPin(port, pin); }

void GPIO_Write(GPIO_TypeDef *port, uint16_t pin, bool state) { HAL_GPIO_WritePin(port, pin, (GPIO_PinState)state); }

void GPIO_Toggle(GPIO_TypeDef *port, uint16_t pin) { HAL_GPIO_TogglePin(port, pin); }

#endif
