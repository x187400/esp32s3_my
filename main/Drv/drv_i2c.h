#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__


#include "driver/i2c_master.h"

typedef enum{
    I2C_XL9555_DEV = 0,
    I2C_ES8311_DEV,
}Drv_I2c_Device_t;

void Drv_I2C_Add_dev(i2c_addr_bit_len_t addr_len, uint16_t addr,uint32_t clk_speed,Drv_I2c_Device_t dev);
void Drv_I2C_Write_Byte(Drv_I2c_Device_t dev, uint8_t reg, uint8_t data, int xfer_timeout_ms);
void Drv_I2C_Read(Drv_I2c_Device_t dev,  uint8_t reg, uint8_t *data, size_t len, int xfer_timeout_ms);

#endif