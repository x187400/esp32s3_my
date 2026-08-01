#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__


#include "driver/i2c_master.h"

typedef enum{
    I2C_XL9555_DEV = 0,
    I2C_ES8311_DEV,
}Drv_I2c_Device_t;

void Drv_I2C_Add_dev(i2c_addr_bit_len_t addr_len, uint16_t addr,uint32_t clk_speed,Drv_I2c_Device_t dev);

/**
 * @brief 向设备指定寄存器写入1字节数据
 * @param dev     I2C 设备枚举
 * @param reg     寄存器地址（8位）
 * @param data    待写入数据指针
 * @param len     数据长度
 * @param timeout 超时时间(ms)
 */
void Drv_I2C_Write_Byte(Drv_I2c_Device_t dev, uint8_t reg, uint8_t data, int xfer_timeout_ms);

/**
 * @brief 从设备指定寄存器读取数据
 * @param dev     I2C 设备枚举
 * @param reg     寄存器地址（8位）
 * @param data    读取数据缓冲区指针
 * @param len     读取长度
 * @param timeout 超时时间(ms)
 */
void Drv_I2C_Read(Drv_I2c_Device_t dev,  uint8_t reg, uint8_t *data, size_t len, int xfer_timeout_ms);

#endif