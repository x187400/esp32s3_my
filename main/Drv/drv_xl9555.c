#include "hal_i2c.h"
#include "proj_cfg.h"
#include "drv_xl9555.h"

static const char *TAG = "Drv_XL9555";

/**
 * @brief 添加i2c设备，将全部寄存器配置为输出，避免触发中断
 * @param none
 * @return none
 */
void Drv_XL9555_Init(void)
{
    Hal_I2C_Add_dev(I2C_ADDR_BIT_LEN_7, XL9555_ADDR, 400000, I2C_XL9555_DEV);
    Drv_XL9555_IO_Cfg(XL9555_AP_INT,XL9555_DIR_OUT);
    Drv_XL9555_IO_Cfg(XL9555_QMA_INT,XL9555_DIR_OUT);
    Drv_XL9555_IO_Cfg(XL9555_BEEP,XL9555_DIR_OUT);
    Drv_XL9555_IO_Cfg(XL9555_KEY1,XL9555_DIR_OUT);
    Drv_XL9555_IO_Cfg(XL9555_KEY0,XL9555_DIR_OUT);
    Drv_XL9555_IO_Cfg(XL9555_SPK_EN,XL9555_DIR_OUT);
    Drv_XL9555_IO_Cfg(XL9555_CTP_RST,XL9555_DIR_OUT);
    Drv_XL9555_IO_Cfg(XL9555_LCD_BL,XL9555_DIR_OUT);

    Drv_XL9555_IO_Cfg(XL9555_LED_R,XL9555_DIR_OUT);
    Drv_XL9555_IO_Cfg(XL9555_CTP_INT,XL9555_DIR_OUT);
    Drv_XL9555_IO_Cfg(XL9555_IO1_2,XL9555_DIR_OUT);
    Drv_XL9555_IO_Cfg(XL9555_IO1_3,XL9555_DIR_OUT);
    Drv_XL9555_IO_Cfg(XL9555_IO1_4,XL9555_DIR_OUT);
    Drv_XL9555_IO_Cfg(XL9555_IO1_5,XL9555_DIR_OUT);
    Drv_XL9555_IO_Cfg(XL9555_IO1_6,XL9555_DIR_OUT);
    Drv_XL9555_IO_Cfg(XL9555_IO1_7,XL9555_DIR_OUT);
}

/**
 * @brief xl9555引脚配置
 * @param port: 表示要配置的端口
 * @param dir:  表示端口的方向，输入或输出
 * @return none
 */
void Drv_XL9555_IO_Cfg(XL9555_Port_e port, Drv_XL9555_Dir_e dir)
{
    uint8_t reg_val;
    uint8_t reg_addr;
    uint8_t bit_pos = port;

    if(port <= XL9555_LCD_BL)
    {
        reg_addr = XL9555_CONFIG_PORT0_REG;
    }
    else
    {
        reg_addr = XL9555_CONFIG_PORT1_REG;
        bit_pos = port - 8;
    }
    Hal_I2C_Read(I2C_XL9555_DEV, reg_addr, &reg_val, 1, 1000);
    if(dir == XL9555_DIR_OUT)
    {
        reg_val &= ~(1 << bit_pos); // 设置为输出
    }
    else
    {
        reg_val |= (1 << bit_pos);  // 设置为输入
    }
    Hal_I2C_Write(I2C_XL9555_DEV, reg_addr, &reg_val, 1,1000);
}


/**
 * @brief xl9555引脚输出设置
 * @param port: 表示要配置的端口
 * @param val:  表示端口的输出值，0或1
 * @return none
 */
void Drv_XL9555_Set_Val(XL9555_Port_e port, uint8_t val)
{
    if(val > 1)
    {
        ESP_LOGE(TAG, "Invalid value for XL9555 port");
        return; // 只允许设置为0或1
    }

    uint8_t reg_val;
    uint8_t reg_addr;
    uint8_t bit_pos = port;

    if(port <= XL9555_LCD_BL)
    {
        reg_addr = XL9555_OUTPUT_PORT0_REG;
    }
    else
    {
        reg_addr = XL9555_OUTPUT_PORT1_REG;
        bit_pos = port - 8;
    }
    Hal_I2C_Read(I2C_XL9555_DEV, reg_addr, &reg_val, 1, 1000);
    if(val == 0)
        reg_val = reg_val & ~(1 << bit_pos);
    else
        reg_val = reg_val | (val << bit_pos);

    Hal_I2C_Write(I2C_XL9555_DEV, reg_addr, &reg_val, 1, 1000);
}

/**
 * @brief xl9555引脚输入读取
 * @param port: 表示要读取的端口
 * @return 端口的输入值，0或1
 */
uint8_t Drv_XL9555_Get_Val(XL9555_Port_e port)
{
    uint8_t val;
    uint8_t bit_pos = (port <= XL9555_LCD_BL) ? port : (port - 8);
    if(port <= XL9555_LCD_BL)
    {
        Hal_I2C_Read(I2C_XL9555_DEV, XL9555_INPUT_PORT0_REG, &val, 1, 1000);
    }
    else
    {
        Hal_I2C_Read(I2C_XL9555_DEV, XL9555_INPUT_PORT1_REG, &val, 1, 1000);
    }
    return (val & (1 << bit_pos)) ? 1 : 0;
}