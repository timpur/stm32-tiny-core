#pragma once

#include <TinyCore.hpp>

#define SHT2x_VALUE_FACTOR 10
#define SHT2x_I2C_ADDR 0x40

#define SHT2x_CMD_TEMPERATURE_NO_HOLD 0xF3
// #define SHT2x_CMD_TEMPERATURE_FOR_HUMIDITY 0xE0
#define SHT2x_CMD_HUMIDITY_NO_HOLD 0xF5
#define SHT2x_CMD_SOFT_RESET 0xFE

// #define SHT2x_CMD_WRITE_USER_REGISTER 0xE6
// #define SHT2x_CMD_READ_USER_REGISTER 0xE7

// #define SHT2x_MASK_RESOLUTION 0x81
// #define SHT2x_MASK_BATTERY 0x20
// #define SHT2x_MASK_HEATER 0x04

class SHT2x {
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
    SHT2x() {}
    bool begin() {
        // uint8_t id[6] = {};
        // if (!I2C_Master_Send_Cmd(SHT2x_I2C_ADDR, SHT2x_REG_READSERIAL))
        //     return false;
        // if (!I2C_Master_Receive(SHT2x_I2C_ADDR, 6, id, sizeof(id)))
        //     return false;
        // Check CRC of the results
        // if (id[2] != crc8(id, 2) || id[5] != crc8(&id[3], 2))
        //     return false;

        // Reset the sensor
        if (!I2C_Master_Send_Cmd(SHT2x_I2C_ADDR, SHT2x_CMD_SOFT_RESET))
            return false;
        delay(15);
        return true;
    }

    // Temp and Humidity is shifted 1 dp, ie 25.0c -> 250
    bool readTempHumidity(int16_t *temperature, int16_t *humidity) {
        uint8_t t_buff[3] = {};
        uint8_t h_buff[3] = {};

        // Get Temp
        if (!I2C_Master_Send_Cmd(SHT2x_I2C_ADDR, SHT2x_CMD_TEMPERATURE_NO_HOLD))
            return false;
        delay(85);
        if (!I2C_Master_Receive(SHT2x_I2C_ADDR, 3, t_buff, sizeof(t_buff)))
            return false;

        // Get Humidity
        if (!I2C_Master_Send_Cmd(SHT2x_I2C_ADDR, SHT2x_CMD_HUMIDITY_NO_HOLD))
            return false;
        delay(29);
        if (!I2C_Master_Receive(SHT2x_I2C_ADDR, 3, h_buff, sizeof(t_buff)))
            return false;

        // Validate
        if (crc8(t_buff, 2) != t_buff[2])
            return false;
        uint16_t t_ticks = ((uint16_t(t_buff[0]) << 8) + uint16_t(t_buff[1])) & ~0x0003;
        if (crc8(h_buff, 2) != h_buff[2])
            return false;
        uint16_t h_ticks = ((uint16_t(h_buff[0]) << 8) + uint16_t(h_buff[1])) & ~0x0003;

        // Parse
        *temperature =
            uint16_t(int(175.72 * SHT2x_VALUE_FACTOR) * int(t_ticks) / 1 << 16) - uint16_t(46.85 * SHT2x_VALUE_FACTOR);
        *humidity =
            uint16_t(int(125.0 * SHT2x_VALUE_FACTOR) * int(h_ticks) / 1 << 16) - uint16_t(6.0 * SHT2x_VALUE_FACTOR);

        return true;
    }
};
