#include "Rtos_msg.h"
#include "Rtos_Queue.h"
#include "Rtos_Task.h"
#include "app_led.h"
#include "app_beep.h"
#include "app_xl9555.h"
#include "app_key.h"
#include "proj_cfg.h"
#include "board.h"
#include "sta_main.h"

static const char *TAG = "Sta_MsgTask";
volatile static bool sta_xl9555_int_flag = false;

static Sta_Main_Event_e staMainEvent = STA_IDLE_EVENT;

static void sta_i2c_int_handler(void *arg)
{
    //xl9555中断处理函数,仅配置为输入的port可进入
    (void)arg;
    sta_xl9555_int_flag = true;
}

static void sta_board_init(void)
{
    I2C_Interupt_Callback_Register(sta_i2c_int_handler);
    I2C_Init();
}

/* ==================== 按键事件触发 ==================== */
static void key0_click_event(void) {App_Led_Set(LED_BLUE,LED_OFF);}
static void key0_double_click_event(void) {App_Led_Set(LED_BLUE,LED_ON);}
static void key0_long_press_event(void) {App_Beep_Set(BEEP_ON);}
static void key0_long_press_release_event(void) {App_Beep_Set(BEEP_OFF);}

static void key1_click_event(void) {}
static void key1_double_click_event(void) {}
static void key1_long_press_event(void) {}
static void key1_long_press_release_event(void) {}

static void key2_click_event(void) {}
static void key2_double_click_event(void) {}
static void key2_long_press_event(void) {}
static void key2_long_press_release_event(void) {}

static const key_event_handle keyTable[KEY_NUM][KEY_MAX_EVENT_NUM] = 
{
    /*             NONE          SINGLE              DOUBLE                     LONG_PRESS             LONG_PRESS_RELEASE     */
    /* KEY0 */ {   NULL,    key0_click_event,   key0_double_click_event,   key0_long_press_event, key0_long_press_release_event },
    /* KEY1 */ {   NULL,    key1_click_event,   key1_double_click_event,   key1_long_press_event, key1_long_press_release_event },
    /* KEY2 */ {   NULL,    key2_click_event,   key2_double_click_event,   key2_long_press_event, key2_long_press_release_event },
};

/* ==================== 消息处理任务 ==================== */
static void Sta_Key_Event(Key_Index_e idx,Key_Event_e evt)
{
    if (idx >= KEY_NUM || evt <= KEY_NONE_EVT || evt >= KEY_MAX_EVENT_NUM) {
        return;
    }
    key_event_handle keyEvt = keyTable[idx][evt];
    if (keyEvt != NULL) 
    {
        keyEvt();
    }
}

static void Sta_Msg_Task(Msg_t msg)
{
    switch (msg.type)
    {
    case MSG_TYPE_KEY_EVENT:
        Sta_Key_Event((Key_Index_e)msg.data.ivalue1,(Key_Event_e)msg.data.ivalue2);
        break;

    default:
        break;
    }
}

/* ==================== 状态机初始化 ==================== */
static void sta_init(void)
{
    #if defined(PERIPHERAL_XL9555_ENABLE) && (PERIPHERAL_XL9555_ENABLE == 1)
        App_XL9555_Init();
    #endif  

    #if defined(PERIPHERAL_LED_ENABLE) && (PERIPHERAL_LED_ENABLE == 1)
        App_Led_Init();
    #endif 

    #if defined(PERIPHERAL_BEEP_ENABLE) && (PERIPHERAL_BEEP_ENABLE == 1)
        App_Beep_Init();
    #endif 

    #if defined(PERIPHERAL_KEY_ENABLE) && (PERIPHERAL_KEY_ENABLE == 1)
        App_Key_Init();
    #endif

    #if defined(PERIPHERAL_E2PROM_ENABLE) && (PERIPHERAL_E2PROM_ENABLE == 1)
        App_E2pRom_Init();
    #endif
    
}

/* ==================== 状态机主任务 ==================== */
static void Sta_main(void)
{
    static bool stakeyInt = false;
    switch (staMainEvent) {
        case STA_IDLE_EVENT:
            if(sta_xl9555_int_flag)
            {
                staMainEvent = STA_XL9555_INT_EVENT;
                sta_xl9555_int_flag = false;
            }

            stakeyInt = App_Key_Int_Trigger_Check();
            if(stakeyInt == true)
            {
                staMainEvent = STA_KEY_EVENT;
            }
            
            break;

        case STA_XL9555_INT_EVENT:
            stakeyInt = App_XL9555_Key_Int_Trigger_Check();
            if(stakeyInt == true)
            {
                staMainEvent = STA_KEY_EVENT;
            }
            else
            {
                staMainEvent = STA_IDLE_EVENT;
            }
            break;


        case STA_KEY_EVENT:
            App_Key_Scan();
            staMainEvent = STA_IDLE_EVENT;
        break;

        default:
            ESP_LOGW(TAG, "Unknown event: %d", staMainEvent);
            staMainEvent = STA_IDLE_EVENT;
            break;
    }
}

/* ==================== 状态机rtos任务 ==================== */
static void Sta_Task(void *pvParameters)
{
    sta_board_init();
    sta_init();
    while (1) {
        Msg_t msg;
        if (Rtos_Queue_Receive(&msg, pdMS_TO_TICKS(10)) == pdPASS) 
        {
            Sta_Msg_Task(msg);
        }
        Sta_main();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/* ==================== rtos任务与消息队列创建 ==================== */
void Sta_Msg_Init(void)
{
    BaseType_t ret = pdPASS;
    ret = Rtos_Queue_Create(RTOS_QUEUE_LEN_SMALL, sizeof(Msg_t));
    if (ret == pdFAIL)
    {
        ESP_LOGE(TAG, "msg queue create failed");
        return;
    }
    Rtos_Task_Create(Sta_Task, "msg_task",
                     RTOS_TASK_STACK_MID, NULL,
                     RTOS_TASK_PRIO_MID, NULL);
}
