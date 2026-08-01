#include "drv_i2c.h"
#include "pin_cfg.h"
#include "app_xl9555.h"

void App_XL9555_Init(void)
{
    Drv_I2C_Add_dev(I2C_ADDR_BIT_LEN_7, XL9555_ADDR, 400000, I2C_XL9555_DEV);
}

void App_XL9555_IO_Cfg(App_XL9555_Dir_e dir)
{
    Drv_I2C_Write_Byte(I2C_XL9555_DEV, XL9555_CONFIG_PORT1_REG, dir, 1000);
}

void App_XL9555_Set_Val(uint8_t val)
{
    uint8_t data = 0xff;
    Drv_I2C_Write_Byte(I2C_XL9555_DEV, XL9555_OUTPUT_PORT1_REG, (data & val), 1000);
}

uint8_t App_XL9555_Get_Val(void)
{
    uint8_t val;
    Drv_I2C_Read(I2C_XL9555_DEV, XL9555_INPUT_PORT1_REG, &val, 1, 1000);
    return (val & 0x01);
}
