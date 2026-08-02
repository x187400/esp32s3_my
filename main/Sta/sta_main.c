#include "Rtos_msg.h"
#include "Rtos_Queue.h"
#include "Rtos_Task.h"
#include "app_led.h"
#include "app_beep.h"
#include "app_xl9555.h"
#include "app_key.h"
#include "proj_cfg.h"
#include "board.h"
#include "driver/gpio.h"
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

// /* ==================== 消息处理任务 ==================== */
static const char *key_evt_str(int32_t evt)
{
    switch (evt)
    {
    case 1: return "single_click";
    case 2: return "double_click";
    case 3: return "long_press";
    case 4: return "long_release";
    default: return "none";
    }
}

static void Sta_Msg_Task(Msg_t msg)
{
    switch (msg.type)
    {
    case MSG_TYPE_KEY_EVENT:
        ESP_LOGI(TAG, "key=%ld, event=%s(%ld)",
                 msg.data.ivalue1, key_evt_str(msg.data.ivalue2), msg.data.ivalue2);
        break;

    default:
        break;
    }
}

static void sta_init(void)
{
    #if defined(STA_XL9555_ENABLE) && (STA_XL9555_ENABLE == 1)
        App_XL9555_Init();
    #endif  

    #if defined(STA_LED_ENABLE) && (STA_LED_ENABLE == 1)
        App_Led_Init();
    #endif 

    #if defined(STA_BEEP_ENABLE) && (STA_BEEP_ENABLE == 1)
        App_Beep_Init();
    #endif 

    #if defined(STA_KEY_ENABLE) && (STA_KEY_ENABLE == 1)
        App_Key_Init();
    #endif
}

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

/* ==================== 初始化 ==================== */

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
