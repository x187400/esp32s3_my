#ifndef __APP_LED_H__
#define __APP_LED_H__

#include "drv_led.h"


void App_Led_Init(void);
void App_Led_Set(Led_Color_e color, Led_State_e state);

#endif