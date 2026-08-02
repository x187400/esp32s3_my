#ifndef __DRV_KEY_H__
#define __DRV_KEY_H__

#define KEY_NUM             3

#define KEY_SCAN_TIME           10      //扫描周期:10ms
#define KEY_DOUBLE_CLICK_CNT    15      //双击时间窗口 KEY_SCAN_TIME * KEY_DOUBLE_CLICK_CNT = 200ms
#define KEY_LONG_PRESS_CNT      60     //长按阈值 KEY_SCAN_TIME * KEY_LONG_PRESS_CNT = 1s

#include "drv_xl9555.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"



typedef enum{
    KEY0,
    KEY1,
    KEY2,
}Key_Index_e;

typedef enum{
    KEY_PRESS,
    KEY_RELEASE,
}Key_State_e;

typedef enum{
    KEY_NONE_EVT,
    KEY_SINGLE_CLICK_EVT,
    KEY_DOUBLE_CLICK_EVT,
    KEY_LONG_PRESS_EVT,
    KEY_LONG_RELEASE_EVT,
}Key_Event_e;

typedef void (*Drv_Key_Event_Cbk)(Key_Index_e idx,Key_Event_e evt);

typedef struct 
{
    uint16_t pressCount;        //按下计数
    uint16_t releaseCount;      //释放计数
    uint8_t clickCount;         //短按计数
    bool longPressed;           //是否触发长按
    Key_Event_e keyEvent;       //按键事件
}Key_Struct_t;

typedef struct
{
    bool keyStart;
    TimerHandle_t keyScanTimer;
}Key_Timer_t;



void Drv_Key_Init(void);
void Drv_Key_Scan_Start(void);
bool Drv_XL9555_Key_Int_Trigger_Check(void);
bool Drv_Key_Int_Trigger_Check(void);
void Drv_Key_Event_Callback_Register(Drv_Key_Event_Cbk cb);

#endif