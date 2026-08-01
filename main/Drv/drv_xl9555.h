#ifndef __DRV_XL9555_H__
#define __DRV_XL9555_H__

#define XL9555_INPUT_PORT0_REG           0 /* 输入P0寄存器用于读取P0端口的输入值 */ 
#define XL9555_INPUT_PORT1_REG           1 /* 输入P1寄存器用于读取P1端口的输入值 */ 
#define XL9555_OUTPUT_PORT0_REG          2 /* 输出P0寄存器用于设置P0端口的输出值 */ 
#define XL9555_OUTPUT_PORT1_REG          3 /* 输出P1寄存器用于设置P1端口的输出值 */ 
#define XL9555_INVERSION_PORT0_REG       4 /* 极性反转P0寄存器用于当P0端口做为输入时，对输入的电平进行反转处理，
                                            即管脚为高电平时，设置这个寄存器中相应的位为1时，读取到的输入寄存器0，1的值就是低电平0 */ 
#define XL9555_INVERSION_PORT1_REG       5 /* 极性反转P1寄存器用于当P1端口做为输入时，对输入的电平进行反转处理，
                                            即管脚为高电平时，设置这个寄存器中相应的位为1时，读取到的输入寄存器0，1的值就是低电平0 */ 
#define XL9555_CONFIG_PORT0_REG          6 /* 配置P0寄存器用于配置P0端口的做为输入(1)或是输出(0) */ 
#define XL9555_CONFIG_PORT1_REG          7 /* 配置P1寄存器用于配置P1端口的做为输入(1)或是输出(0) */

#include <stdint.h>

typedef enum{
    XL9555_DIR_OUT = 0,
    XL9555_DIR_IN,
}Drv_XL9555_Dir_e;

typedef enum{
    XL9555_AP_INT = 0,
    XL9555_QMA_INT,
    XL9555_BEEP,
    XL9555_KEY1,
    XL9555_KEY0,
    XL9555_SPK_EN,
    XL9555_CTP_RST,
    XL9555_LCD_BL,
    XL9555_LED_R,
    XL9555_CTP_INT,
    XL9555_IO1_2,
    XL9555_IO1_3,
    XL9555_IO1_4,
    XL9555_IO1_5,
    XL9555_IO1_6,
    XL9555_IO1_7,
}XL9555_Port_e;

void Drv_XL9555_Init(void);
void Drv_XL9555_IO_Cfg(XL9555_Port_e port, Drv_XL9555_Dir_e dir);
void Drv_XL9555_Set_Val(XL9555_Port_e port, uint8_t val);
uint8_t Drv_XL9555_Get_Val(XL9555_Port_e port);

#endif