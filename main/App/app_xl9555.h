#ifndef __APP_XL9555_H__
#define __APP_XL9555_H__

#include "drv_xl9555.h"

void App_XL9555_Init(void);
void App_XL9555_Set_Val(XL9555_Port_e port, uint8_t val);
uint8_t App_XL9555_Get_Val(XL9555_Port_e port);

#endif