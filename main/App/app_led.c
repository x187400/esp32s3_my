
#include "app_led.h"

void App_Led_Init(void)
{
    Drv_Led_Init();
}

void App_Led_Set(Led_Color_e color, Led_State_e state)
{
    Drv_Led_Set_State(color, state);
}