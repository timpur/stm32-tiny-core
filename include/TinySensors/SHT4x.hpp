#pragma once

#include <TinyCore.hpp>

#define SHT4x_VALUE_FACTOR 10
#define SHT4x_I2C_ADDR 0x44
// #define SHT4x_I2C_ADDR 0x45
// #define SHT4x_I2C_ADDR 0x46

#define SHT4x_REG_NOHEAT_HIGHPRECISION 0xFD
#define SHT4x_REG_NOHEAT_MEDPRECISION 0xF6
#define SHT4x_REG_NOHEAT_LOWPRECISION 0xE0

#define SHT4x_REG_HIGHHEAT_1S 0x39
#define SHT4x_REG_HIGHHEAT_100MS 0x32
#define SHT4x_REG_MEDHEAT_1S 0x2F
#define SHT4x_REG_MEDHEAT_100MS 0x24
#define SHT4x_REG_LOWHEAT_1S 0x1E
#define SHT4x_REG_LOWHEAT_100MS 0x15

#define SHT4x_REG_READSERIAL 0x89
#define SHT4x_REG_SOFTRESET 0x94

class SHT4x {
  private:
    static uint8_t crc8(const uint8_t *data, int len) {
        const uint8_t POLYNOMIAL = 0x31;
        uint8_t crc(0xFF);
        for (int j = len; j; --j) {
            crc ^= *data++;
            for (int i = 8; i; --i) {
                crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
            }
        }
        return crc;
    }

  public:
    SHT4x() {}
    bool begin() {
        uint8_t id[6] = {};
        if (!I2C_Master_Send_Cmd(SHT4x_I2C_ADDR, SHT4x_REG_READSERIAL))
            return false;
        if (!I2C_Master_Receive(SHT4x_I2C_ADDR, 6, id, sizeof(id)))
            return false;
        // Check CRC of the results
        if (id[2] != crc8(id, 2) || id[5] != crc8(&id[3], 2))
            return false;

        // Reset the sensor
        if (!I2C_Master_Send_Cmd(SHT4x_I2C_ADDR, SHT4x_REG_SOFTRESET))
            return false;
        delay(1);
        return true;
    }

    // Temp and Humidity is shifted 1 dp, ie 25.0c -> 250
    bool readTempHumidity(int16_t *temperature, int16_t *humidity, uint8_t cmd = SHT4x_REG_NOHEAT_HIGHPRECISION) {
        uint8_t buff[6] = {};
        uint16_t ms;
        switch (cmd) {
        case SHT4x_REG_NOHEAT_HIGHPRECISION:
            ms = 10;
            break;
        case SHT4x_REG_NOHEAT_MEDPRECISION:
            ms = 6;
            break;
        case SHT4x_REG_NOHEAT_LOWPRECISION:
            ms = 3;
            break;
        case SHT4x_REG_HIGHHEAT_1S:
        case SHT4x_REG_MEDHEAT_1S:
        case SHT4x_REG_LOWHEAT_1S:
            ms = 1100;
            break;
        case SHT4x_REG_HIGHHEAT_100MS:
        case SHT4x_REG_MEDHEAT_100MS:
        case SHT4x_REG_LOWHEAT_100MS:
            ms = 110;
            break;
        }
        // Send command with the right wait to read the results
        if (!I2C_Master_Send_Cmd(SHT4x_I2C_ADDR, cmd))
            return false;
        delay(ms);
        if (!I2C_Master_Receive(SHT4x_I2C_ADDR, 6, buff, sizeof(buff)))
            return false;

        // Check CRC of the results
        if (buff[2] != crc8(buff, 2) || buff[5] != crc8(&buff[3], 2))
            return false;

        uint16_t t_ticks = (uint16_t(buff[0]) << 8) + uint16_t(buff[1]);
        uint16_t rh_ticks = (uint16_t(buff[3]) << 8) + uint16_t(buff[4]);
        *temperature = int16_t((175 * int(t_ticks) * SHT4x_VALUE_FACTOR / UINT16_MAX) - (45 * SHT4x_VALUE_FACTOR));
        *humidity = int16_t((125 * int(rh_ticks) * SHT4x_VALUE_FACTOR / UINT16_MAX) - (6 * SHT4x_VALUE_FACTOR));

        return true;
    }
};
