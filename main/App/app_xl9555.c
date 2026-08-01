#include "app_xl9555.h"

void App_XL9555_Init(void)
{
    Drv_XL9555_Init();
}

void App_XL9555_Set_Val(XL9555_Port_e port, uint8_t val)
{
    Drv_XL9555_Set_Val(port, val);
}

uint8_t App_XL9555_Get_Val(XL9555_Port_e port)
{
    return Drv_XL9555_Get_Val(port);
}