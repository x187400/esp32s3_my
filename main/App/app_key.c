#include "drv_key.h"
#include "app_key.h"

static const char *TAG = "App_Key";


void App_Key_Init(void)
{
    Drv_Key_Init();
}

void App_Key_Scan(void)
{
    Drv_Key_Scan();
}

bool App_XL9555_Key_Int_Trigger_Check(void)
{
    return Drv_XL9555_Key_Int_Trigger_Check();
}

bool App_Key_Int_Trigger_Check(void)
{
    return Drv_Key_Int_Trigger_Check();
}