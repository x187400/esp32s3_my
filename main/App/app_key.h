#ifndef __APP_KEY_H__
#define __APP_KEY_H__

#include "drv_key.h"

void App_Key_Init(void);
void App_Key_Scan(void);
bool App_XL9555_Key_Int_Trigger_Check(void);
bool App_Key_Int_Trigger_Check(void);

#endif