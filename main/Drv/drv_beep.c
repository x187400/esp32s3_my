#include "drv_xl9555.h"
#include "drv_beep.h"

void Drv_Beep_Init(void)
{
    Drv_XL9555_IO_Cfg(XL9555_BEEP, XL9555_DIR_OUT);
    Drv_XL9555_Set_Val(XL9555_BEEP, 1);
}

void Drv_Beep_Set_State(Beep_State_e state)
{
    Drv_XL9555_Set_Val(XL9555_BEEP, state);
}