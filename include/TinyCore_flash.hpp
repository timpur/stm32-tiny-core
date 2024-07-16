#pragma once

#include <stm32g0xx_hal_flash.h>
#include <stm32g0xx_hal_flash_ex.h>
#include <TinyCore_config.hpp>

#ifdef HAL_FLASH_MODULE_ENABLED

#ifdef FLASH_BANK_2
#define FLASH_BANK_NUMBER FLASH_BANK_2
#else
#define FLASH_BANK_NUMBER FLASH_BANK_1
#endif

#define FLASH_PAGE_NUMBER uint32_t(FLASH_PAGE_NB - 1)

#define FLASH_START uint32_t(FLASH_BASE + (FLASH_PAGE_NUMBER * FLASH_PAGE_SIZE))

// #ifndef FLASH_END
// #if defined FLASH_BANK2_END && (FLASH_BANK_NUMBER == FLASH_BANK_2)
// #define FLASH_END FLASH_BANK2_END
// #elif defined FLASH_BANK1_END && (FLASH_BANK_NUMBER == FLASH_BANK_1)
// #define FLASH_END FLASH_BANK1_END
// #else
// #define FLASH_END ((uint32_t)(FLASH_BASE + ((FLASH_PAGE_NB * FLASH_PAGE_SIZE)) - 1))
// #endif
// #endif

void FLASH_Init() {}

bool FLASH_Erase() {
    if (HAL_FLASH_Unlock() != HAL_OK)
        return false;

    if (FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE) != HAL_OK)
        goto error;

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR);
    FLASH_PageErase(FLASH_BANK_NUMBER, FLASH_PAGE_NUMBER);

    if (FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE) != HAL_OK)
        goto error;

    CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
    HAL_FLASH_Lock();
    return true;
error:
    HAL_FLASH_Lock();
    return false;
}

void FLASH_Write(uint8_t *data, size_t size) {
    FLASH_Erase();

}

void FLASH_Read() {}

#endif