#pragma once

#include <stm32g0xx_hal_i2c.h>
#include <TinyCore_config.hpp>
#include <TinyCore_sys.hpp>
#include <TinyCore_gpio.hpp>

I2C_HandleTypeDef I2cHandle = {
    .Instance = I2C1,
    .Init =
        {
            .Timing = 0x10707DBC,
            .OwnAddress1 = 0,
            .AddressingMode = I2C_ADDRESSINGMODE_7BIT,
            .DualAddressMode = I2C_DUALADDRESS_DISABLE,
            .OwnAddress2 = 0,
            .OwnAddress2Masks = I2C_OA2_NOMASK,
            .GeneralCallMode = I2C_GENERALCALL_DISABLE,
            .NoStretchMode = I2C_NOSTRETCH_DISABLE,
        },
};

void I2C_Init()
{
    if (HAL_I2C_Init(&I2cHandle) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_I2CEx_ConfigAnalogFilter(&I2cHandle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_I2CEx_ConfigDigitalFilter(&I2cHandle, 0) != HAL_OK)
    {
        Error_Handler();
    }
}

bool I2C_Master_IsDeviceReady(uint16_t addr, uint32_t timeout = 100)
{
    if (HAL_I2C_IsDeviceReady(&I2cHandle, addr << 1, 3, timeout) != HAL_OK)
    {
        // if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF) {
        return false;
        // }
    }
    return true;
}

bool I2C_Master_Send(uint16_t addr, uint8_t *data, size_t data_len, uint32_t timeout = 100)
{
    if (HAL_I2C_Master_Transmit(&I2cHandle, addr << 1, data, data_len, timeout) != HAL_OK)
    {
        // if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF) {
        return false;
        // }
    }
    return true;
}

bool I2C_Master_Receive(uint16_t addr, uint8_t reg_size, uint8_t *data, size_t data_len, uint32_t timeout = 100)
{
    if (reg_size > data_len)
        return false;
    if (HAL_I2C_Master_Receive(&I2cHandle, addr << 1, data, data_len, timeout) != HAL_OK)
    {
        // if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF) {
        return false;
        // }
    }
    return true;
}

bool I2C_Master_Send_Cmd(uint16_t addr, uint8_t cmd, uint32_t timeout = 100)
{
    uint8_t buff[] = {cmd};
    return I2C_Master_Send(addr, buff, sizeof(buff), timeout);
}

bool I2C_Master_Write_Reg(uint16_t addr, uint8_t reg, uint8_t *data, uint8_t data_len, uint32_t timeout = 100)
{
    if (HAL_I2C_Mem_Write(&I2cHandle, addr << 1, reg, I2C_MEMADD_SIZE_8BIT, data, data_len, timeout) != HAL_OK)
    {
        // if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF) {
        return false;
        // }
    }
    return true;
}

bool I2C_Master_Receive_Reg(uint16_t addr, uint8_t reg, uint8_t reg_size, uint8_t *data, uint8_t data_len,
                            uint32_t timeout = 100)
{
    if (reg_size > data_len)
        return false;
    if (HAL_I2C_Mem_Read(&I2cHandle, addr << 1, reg, I2C_MEMADD_SIZE_8BIT, data, reg_size, timeout) != HAL_OK)
    {
        // if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF) {
        return false;
        // }
    }
    return true;
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        RCC_PeriphCLKInitTypeDef periphClkInit = {0};
        periphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
        periphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
        HAL_RCCEx_PeriphCLKConfig(&periphClkInit);

        GPIO_Mode_AlternateFn(GPIOB, GPIO_PIN_9 | GPIO_PIN_8, GPIO_AF6_I2C1, GPIO_PULLUP, true);

        __HAL_RCC_I2C1_CLK_ENABLE();
    }
}
