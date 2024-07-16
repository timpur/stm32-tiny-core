#pragma once

#include <TinyCore_onewire.hpp>

#define DS18B20_CONVERT_TEMP 0x44
#define DS18B20_READ_SIRACH_PAD 0xBE

class DS18B20 {
  private:
    OneWire_HandleTypeDef *_ow;

  public:
    void setup(OneWire_HandleTypeDef *onewire) {
        _ow = onewire;
        if (OneWire_Reset(_ow)) {
            OneWire_WriteByte(_ow, ONEWIRE_CMD_SKIPROM);
            OneWire_WriteByte(_ow, DS18B20_CONVERT_TEMP);
        }
    }

    int32_t value() {
        if (OneWire_Reset(_ow)) {
            OneWire_WriteByte(_ow, ONEWIRE_CMD_SKIPROM);
            OneWire_WriteByte(_ow, DS18B20_CONVERT_TEMP);
        }
        if (OneWire_Reset(_ow)) {
            OneWire_WriteByte(_ow, ONEWIRE_CMD_SKIPROM);
            OneWire_WriteByte(_ow, DS18B20_READ_SIRACH_PAD);
        }

        uint8_t lsb = OneWire_ReadByte(_ow);
        uint8_t msb = OneWire_ReadByte(_ow);
        int32_t temp = ((msb << 8)) | lsb;
        temp *= 100;
        temp >>= 4;
        delay(750);
        return temp;
    }
};