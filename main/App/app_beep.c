#include "app_beep.h"

void App_Beep_Init(void)
{
    Drv_Beep_Init();
}

void App_Beep_Set(Beep_State_e state)
{
    Drv_Beep_Set_State(state);
}