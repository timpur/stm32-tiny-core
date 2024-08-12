#pragma once

#include <TinyCore.hpp>

#define AHT20_VALUE_FACTOR 10
#define AHT20_I2C_ADDR 0x38

#define AHT20_CMD_CALIBRATE 0xBE
#define AHT20_CMD_TRIGGER 0xAC
#define AHT20_CMD_SOFT_RESET 0xBA

#define AHT20_MASK_STATUS_BUSY 0x80
#define AHT20_MASK_STATUS_CALIBRATED 0x08

class AHT20
{
private:
    static uint8_t crc8(const uint8_t *data, int len)
    {
        const uint8_t POLYNOMIAL = 0x31;
        uint8_t crc = 0xFF;
        for (int j = len; j; --j)
        {
            crc ^= *data++;
            for (int i = 8; i; --i)
            {
                crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
            }
        }
        return crc;
    }

    uint8_t _getStatus()
    {
        volatile uint8_t res = 0xFF;
        I2C_Master_Receive(AHT20_I2C_ADDR, 1, (uint8_t *)&res, 1);
        return res;
    }

public:
    AHT20() {}
    bool begin()
    {
        // Test Coms
        if (!I2C_Master_IsDeviceReady(AHT20_I2C_ADDR))
            return false;

        // Reset the sensor
        if (!I2C_Master_Send_Cmd(AHT20_I2C_ADDR, AHT20_CMD_SOFT_RESET))
            return false;
        delay(20);

        while (_getStatus() & AHT20_MASK_STATUS_BUSY)
            delay(10);

        uint8_t cmd[] = {AHT20_CMD_CALIBRATE, 0x08, 0x00};
        I2C_Master_Send(AHT20_I2C_ADDR, (uint8_t *)&cmd, sizeof(cmd));
        delay(10);

        while (_getStatus() & AHT20_MASK_STATUS_BUSY)
            delay(10);

        if (!(_getStatus() & AHT20_MASK_STATUS_CALIBRATED))
            return false;

        return true;
    }

    // Temp and Humidity is shifted 1 dp, ie 25.0c -> 250
    bool readTempHumidity(int16_t *temperature, int16_t *humidity)
    {
        uint8_t cmd[3] = {AHT20_CMD_TRIGGER, 0x33, 0};
        if (!I2C_Master_Send(AHT20_I2C_ADDR, (uint8_t *)&cmd, sizeof(cmd)))
            return false;
        // delay(80);

        while (_getStatus() & AHT20_MASK_STATUS_BUSY)
            delay(10);

        uint8_t buff[7] = {};
        if (!I2C_Master_Receive(AHT20_I2C_ADDR, 7, (uint8_t *)&buff, sizeof(buff)))
            return false;

        uint32_t rh_ticks = (uint32_t(buff[1]) << 12) + (uint32_t(buff[2]) << 4) + (uint32_t(buff[3]) >> 4);
        uint32_t t_ticks = (uint32_t(buff[3] & 0x0F) << 16) + (uint32_t(buff[4]) << 8) + uint32_t(buff[5]);

        *humidity = int16_t((int64_t(rh_ticks) * 100 * AHT20_VALUE_FACTOR) / 0x100000);
        *temperature = int16_t(((int64_t(t_ticks) * 200 * AHT20_VALUE_FACTOR) / 0x100000) - (50 * AHT20_VALUE_FACTOR));

        return true;
    }
};
