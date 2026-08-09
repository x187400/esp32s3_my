#ifndef __HAL_I2C_H__
#define __HAL_I2C_H__


#include "driver/i2c_master.h"

typedef enum{
    I2C_XL9555_DEV = 0,
    I2C_ES8311_DEV,
    I2C_E2PROM_DEV,
    I2C_TOUCH_DEV,
}Hal_I2c_Device_t;

void Hal_I2C_Add_dev(i2c_addr_bit_len_t addr_len, uint16_t addr,uint32_t clk_speed,Hal_I2c_Device_t dev);
esp_err_t Hal_I2C_Write(Hal_I2c_Device_t dev, uint8_t reg, const uint8_t *data, size_t len, int xfer_timeout_ms);
esp_err_t Hal_I2C_Read(Hal_I2c_Device_t dev, uint8_t reg, uint8_t *data, size_t len, int xfer_timeout_ms);
esp_err_t Hal_I2C_Read_Addr32(Hal_I2c_Device_t dev, uint32_t addr, uint8_t *data, size_t len, int xfer_timeout_ms);
esp_err_t Hal_I2C_Write_Addr32(Hal_I2c_Device_t dev, uint32_t addr, const uint8_t *data, size_t len, int xfer_timeout_ms);

#endif
