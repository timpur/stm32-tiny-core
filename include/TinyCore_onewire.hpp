#pragma once

#include <TinyCore_sys.hpp>
#include <TinyCore_gpio.hpp>

typedef struct __OneWire_HandleTypeDef {
    GPIO_TypeDef *port;
    uint16_t pin;
} OneWire_HandleTypeDef;

#define ONEWIRE_OUTPUT(how) GPIO_Mode_Output(how->port, how->pin, true, GPIO_PULLUP, GPIO_SPEED_FREQ_MEDIUM)
// #define ONEWIRE_INPUT(how) GPIO_Mode_Input(how->port, how->pin, GPIO_PULLUP)
#define ONEWIRE_SET(how, val) GPIO_Write(how->port, how->pin, val)
#define ONEWIRE_READ(how) GPIO_Read(how->port, how->pin)

/* OneWire commands */
#define ONEWIRE_CMD_RSCRATCHPAD 0xBE
#define ONEWIRE_CMD_WSCRATCHPAD 0x4E
#define ONEWIRE_CMD_CPYSCRATCHPAD 0x48
#define ONEWIRE_CMD_RECEEPROM 0xB8
#define ONEWIRE_CMD_RPWRSUPPLY 0xB4
#define ONEWIRE_CMD_SEARCHROM 0xF0
#define ONEWIRE_CMD_READROM 0x33
#define ONEWIRE_CMD_MATCHROM 0x55
#define ONEWIRE_CMD_SKIPROM 0xCC

void OneWire_Init(OneWire_HandleTypeDef *how) { ONEWIRE_OUTPUT(how); }

bool OneWire_Reset(OneWire_HandleTypeDef *how) {
    bool i;

    /* Line low, and wait 480us */
    ONEWIRE_SET(how, false);
    delay_us(480);

    /* Release line and wait for 70us */
    ONEWIRE_SET(how, true);
    delay_us(70);

    /* Check bit value */
    i = ONEWIRE_READ(how);

    /* Delay for 410 us */
    delay_us(410);

    /* Return value of presence pulse, 0 = OK, 1 = ERROR */
    return i;
}

void OneWire_WriteBit(OneWire_HandleTypeDef *how, bool bit) {
    if (bit) {
        /* Set line low */
        ONEWIRE_SET(how, false);
        delay_us(6);

        /* Wait for 55 us and release the line */
        ONEWIRE_SET(how, true);
        delay_us(55);
    } else {
        /* Set line low */
        ONEWIRE_SET(how, false);
        delay_us(65);

        /* Wait for 5 us and release the line */
        ONEWIRE_SET(how, true);
        delay_us(5);
    }
}

bool OneWire_ReadBit(OneWire_HandleTypeDef *how) {
    bool bit = 0;

    /* Line low */
    ONEWIRE_SET(how, false);
    delay_us(3);

    /* Release line */
    ONEWIRE_SET(how, true);
    delay_us(10);

    /* Read line value */
    if (ONEWIRE_READ(how)) {
        /* Bit is HIGH */
        bit = 1;
    }

    /* Wait 50us to complete 60us period */
    delay_us(50);

    /* Return bit value */
    return bit;
}

void OneWire_WriteByte(OneWire_HandleTypeDef *how, uint8_t byte) {
    uint8_t i = 8;
    /* Write 8 bits */
    while (i--) {
        /* LSB bit is first */
        OneWire_WriteBit(how, byte & 0x01);
        byte >>= 1;
    }
}

uint8_t OneWire_ReadByte(OneWire_HandleTypeDef *how) {
    uint8_t i = 8, byte = 0;
    while (i--) {
        byte >>= 1;
        byte |= (OneWire_ReadBit(how) << 7);
    }

    return byte;
}
