#include "Rtos_Queue.h"
#include "app_key.h"

static const char *TAG = "App_Key";

static void app_key_event_callback(Key_Index_e idx,Key_Event_e evt)
{
    Msg_t msg;
    msg.type = MSG_TYPE_KEY_EVENT;
    msg.data.ivalue1 = idx;
    msg.data.ivalue2 = evt;
    Rtos_Queue_Send(&msg,pdMS_TO_TICKS(10));
}

void App_Key_Init(void)
{
    Drv_Key_Init();
    Drv_Key_Event_Callback_Register(app_key_event_callback);
}

void App_Key_Scan(void)
{
    Drv_Key_Scan_Start();
}

bool App_XL9555_Key_Int_Trigger_Check(void)
{
    return Drv_XL9555_Key_Int_Trigger_Check();
}

bool App_Key_Int_Trigger_Check(void)
{
    return Drv_Key_Int_Trigger_Check();
}