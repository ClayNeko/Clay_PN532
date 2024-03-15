#ifndef __CLAY_PN532_I2C_H__
#define __CLAY_PN532_I2C_H__

#include "driver/i2c.h"
#define I2C_IO_SDA 4
#define I2C_IO_SCL 5
#define PN532_SLAVE_ADDR 0x48

class Clay_PN532_I2C
{
private:
    esp_err_t err;
    i2c_config_t i2ccfg;
public:
    esp_err_t init();
    int wake();
    esp_err_t write(const uint8_t* data, size_t datasize);
    uint8_t read();
};

#endif